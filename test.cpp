#include <iostream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
#include <chrono>
#include <sstream>

struct Message {
    uint8_t type;
    uint32_t player_id;
    uint32_t game_id;
    uint8_t pawn;
};

std::vector<uint8_t> serialize(const Message& msg, int len) {
    std::vector<uint8_t> buf(len, 0);
    buf[0] = msg.type;
    uint32_t pid = htonl(msg.player_id);
    uint32_t gid = htonl(msg.game_id);
    if (len >= 5) std::memcpy(&buf[1], &pid, 4);
    if (len >= 9) std::memcpy(&buf[5], &gid, 4);
    if (len >= 10) buf[9] = msg.pawn;
    return buf;
}

std::string board_to_string(const std::vector<uint8_t>& res) {
    if (res.size() < 14) return "";
    uint8_t max_pawn = res[13];
    std::string s = "";
    for (int i = 0; i <= max_pawn; ++i) {
        int byte_idx = 14 + (i / 8);
        int bit_idx = 7 - (i % 8);
        if (byte_idx < (int)res.size() && (res[byte_idx] & (1 << bit_idx))) s += "I";
        else s += ".";
    }
    return s;
}

std::string status_name(int s) {
    switch(s) {
        case 0: return "WAITING(0)";
        case 1: return "TURN_A(1)";
        case 2: return "TURN_B(2)";
        case 3: return "WIN_A(3)";
        case 4: return "WIN_B(4)";
        case 255: return "ERROR(255)";
        default: return "UNKNOWN(" + std::to_string(s) + ")";
    }
}

class KaylesTester {
    int sock;
    struct sockaddr_in servaddr;
public:
    KaylesTester() {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(2020);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 300000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }
    ~KaylesTester() { close(sock); }

    std::vector<uint8_t> send_and_recv(const std::vector<uint8_t>& out) {
        sendto(sock, out.data(), out.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
        std::vector<uint8_t> in(2048);
        socklen_t len = sizeof(servaddr);
        ssize_t n = recvfrom(sock, in.data(), in.size(), 0, (struct sockaddr*)&servaddr, &len);
        if (n < 0) return {};
        in.resize(n);
        return in;
    }

    uint32_t get_game_id(const std::vector<uint8_t>& res) {
        if (res.size() < 4) return 0;
        uint32_t gid; std::memcpy(&gid, &res[0], 4);
        return ntohl(gid);
    }
};

bool assert_res(const std::vector<uint8_t>& res, bool condition, const std::string& msg, const std::string& expected) {
    if (condition) {
        std::cout << "[PASS] " << msg << std::endl;
        return true;
    } else {
        std::cout << "[FAIL] " << msg << "\n"
                  << "       Expected: " << expected << "\n"
                  << "       Actual:   len=" << res.size();
        if (res.size() >= 13) std::cout << ", status=" << status_name(res[12]);
        if (res.size() >= 14) {
            if (res[12] == 255) std::cout << ", error_idx=" << (int)res[13];
            else std::cout << ", max_pawn=" << (int)res[13] << ", board=" << board_to_string(res);
        }
        std::cout << "\n       Hex:      ";
        for (auto b : res) printf("%02x ", b);
        std::cout << "\n" << std::endl;
        return false;
    }
}

void check(bool condition, const std::string& msg) {
    if (condition) std::cout << "[PASS] " << msg << std::endl;
    else std::cout << "[FAIL] " << msg << std::endl;
}

int main() {
    KaylesTester tester;
    uint32_t p1 = 111, p2 = 222, p3 = 333;
    uint32_t g1 = 0;

    std::cout << "--- Section 1: Protocol Basics & Error Indices ---" << std::endl;
    auto res = tester.send_and_recv({});
    assert_res(res, res.size() >= 14 && res[12] == 255 && res[13] == 0, "Zero-length packet", "MSG_WRONG_MSG, error_idx 0");
    
    res = tester.send_and_recv(serialize({0, 0, 0, 0}, 5));
    assert_res(res, res.size() >= 14 && res[12] == 255 && res[13] == 1, "JOIN with player_id 0", "MSG_WRONG_MSG, error_idx 1");

    res = tester.send_and_recv(serialize({0, p1, 0, 0}, 4));
    assert_res(res, res.size() >= 14 && res[12] == 255 && res[13] == 4, "JOIN with truncated player_id (4 bytes)", "MSG_WRONG_MSG, error_idx 4");

    std::cout << "\n--- Section 2: Core Game Logic ---" << std::endl;
    res = tester.send_and_recv(serialize({0, p1, 0, 0}, 5));
    if (!assert_res(res, res.size() >= 14 && res[12] == 0, "Player 1 JOIN fresh game", "Status WAITING(0)")) return 1;
    g1 = tester.get_game_id(res);
    uint8_t max_p = res[13];

    res = tester.send_and_recv(serialize({0, p2, 0, 0}, 5));
    assert_res(res, res.size() >= 14 && res[12] == 2 && tester.get_game_id(res) == g1, "Player 2 joins same game", "Status TURN_B(2), same ID");

    res = tester.send_and_recv(serialize({1, p1, g1, 0}, 10)); // Player A tries to move during TURN_B
    assert_res(res, res.size() >= 14 && res[12] == 2, "Move out of turn (Player A during TURN_B)", "Status remains TURN_B(2)");

    res = tester.send_and_recv(serialize({4, p1, g1, 0}, 9)); // Player A tries to give up during TURN_B
    assert_res(res, res.size() >= 14 && res[12] == 2, "GIVE_UP out of turn", "Status remains TURN_B(2)");

    res = tester.send_and_recv(serialize({1, p2, g1, 0}, 10));
    assert_res(res, res.size() >= 14 && res[12] == 1, "Legal MOVE_1 by Player B", "Status becomes TURN_A(1)");

    std::cout << "\n--- Section 3: Isolation & Multi-player ---" << std::endl;
    res = tester.send_and_recv(serialize({0, p3, 0, 0}, 5)); 
    uint32_t g2 = tester.get_game_id(res);
    check(g2 != g1, "Fresh game has unique ID: " + std::to_string(g2));

    res = tester.send_and_recv(serialize({1, p1, g2, 0}, 10));
    assert_res(res, res.size() >= 14 && res[12] == 255 && res[13] == 1, "Illegal participant in g2", "ERROR(255), error_idx 1");

    std::cout << "\n--- Section 4: Stress Test (1000 Games) ---" << std::endl;
    int count = 0;
    auto start = std::chrono::steady_clock::now();
    for(int i=0; i<1000; ++i) {
        res = tester.send_and_recv(serialize({0, (uint32_t)(5000+i), 0, 0}, 5));
        if (res.size() >= 14 && res[12] != 255) count++;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Created " << count << " games in " << diff.count() << "s" << std::endl;
    check(count == 1000, "Server stable after 1000 games");

    std::cout << "\n--- Section 5: Bitmask Integrity ---" << std::endl;
    res = tester.send_and_recv(serialize({3, p1, g1, 0}, 9));
    if (res.size() >= 14) {
        int bitmap_bytes = (max_p / 8) + 1;
        check((int)res.size() == 14 + bitmap_bytes, "Packet length matches bitmap size");
        uint8_t last_byte = res[14 + (max_p / 8)];
        uint8_t mask = 0;
        for (int i = (max_p % 8) + 1; i < 8; ++i) mask |= (1 << (7 - i));
        check((last_byte & mask) == 0, "Extra bits in last bitmap byte are zeroed");
    }

    std::cout << "\n--- All Extended Tests Finished ---" << std::endl;
    return 0;
}
