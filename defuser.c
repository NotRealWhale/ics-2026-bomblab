#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t low = (uint32_t)challenge;
    uint32_t shifted = (uint32_t)(challenge >> 16);
    uint32_t mid1 = (high * 5u) ^ (shifted + 0x6b8bu);
    uint32_t mid0 = (high ^ 0x5d40u) + low * 4u;
    uint32_t answer = (mid0 * 3u + mid1) ^ 0xe025u;
    values[0] = answer;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;

    values[0] = (uint32_t)((challenge >> 45) & 0xfu) ^ 0xdu;
    for (size_t i = 1; i < count; ++i) {
        unsigned shift = 22u + 3u * (unsigned)(i - 1);
        uint32_t part = (uint32_t)((challenge >> shift) & 0x1fu);
        uint32_t previous_times_three = values[i - 1] * 3u;

        values[i] = (part + (uint32_t)i) ^ previous_times_three;
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    uint32_t index = (uint32_t)challenge & 7u;
    uint32_t x = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t answer = 0;

    switch (index) {
        case 0:
            answer = x * 5u + 0x49c9u;
            break;
        case 1:
            answer = x * 5u + 0xf483u;
            break;
        case 2:
            answer = (x ^ 0x9f5du) + (x >> 2);
            break;
        case 3:
            answer = x * 5u + 0x4b5b4u;
            break;
        case 4:
            answer = (x * 4u) ^ 0x624u;
            break;
        case 5:
            answer = x * 3u + 0x6047u;
            break;
        case 6:
            answer = (x ^ 0xb16au) + (x >> 1);
            break;
        case 7:
            answer = (x * 4u) ^ 0x6fcu;
            break;
    }

    values[0] = index;
    values[1] = answer;
    return write_answer(out, cap, values, count);
}

static uint32_t walk4(uint32_t target, uint32_t low, uint32_t high, uint32_t key)
{
    uint32_t mid = low + ((high - low) >> 1);

    if (target == mid) {
        return (mid + 0xedu) ^ key;
    }

    uint32_t saved = mid + key;
    uint32_t child;

    if (target < mid) {
        child = walk4(target, low, mid - 1u, key);
        return (child * 3u) ^ saved ^ 0x17u;
    } else {
        child = walk4(target, mid + 1u, high, key);
        return (child * 3u) ^ saved ^ 0x4u;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t target = (uint32_t)challenge & 0x7fu;
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);

    values[0] = walk4(target, 0u, 127u, key);
    return write_answer(out, cap, values, count);
}


struct node5 {
    unsigned next;
    uint32_t id;
    uint32_t weight;
};

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    static const struct node5 nodes[] = {
        {5,  0x32u,  0x43d6u}, {4,  0x25du, 0x9ef9u},
        {3,  0x652u, 0x36ebu}, {0,  0x442u, 0x10ccu},
        {7,  0x50bu, 0xfe10u}, {8,  0x17bu, 0xd964u},
        {1,  0xb4fu, 0x5ee7u}, {2,  0x929u, 0x5b7bu},
        {11, 0x728u, 0x47bfu}, {10, 0x81cu, 0xbcc7u},
        {12, 0x357u, 0x88b8u}, {9,  0xa57u, 0xbd00u}
    };
    static const unsigned heads[8] = {6, 1, 4, 7, 2, 3, 0, 5};
    unsigned node = heads[(unsigned)(challenge & 7u)];
    unsigned visits = 1u + (unsigned)((challenge >> 3) & 3u);
    uint32_t acc = (uint32_t)(challenge >> 6)
                 ^ (uint32_t)(challenge >> 32) ^ 0xd4b4u;
    uint32_t last_id = 0;
    for (unsigned i = 0; i < visits; ++i) {
        if (node >= sizeof(nodes) / sizeof(nodes[0])) return 1;
        acc = (acc + nodes[node].weight) * 5u;
        last_id = nodes[node].id;
        acc ^= last_id;
        node = nodes[node].next;
    }
    uint32_t values[2] = {acc, last_id};
    return write_answer(out, cap, values, 2);
}

static uint32_t apply6(unsigned index, uint32_t x) {

    switch (index) {
        case 0: return (x ^ 0x4f79u) + (x << 5);
        case 1: return (x >> 1) | (x << 31);
        case 2: return ((x << 5) | (x >> 27)) ^ 0x9532u;
        case 3:
        case 5: return ((x >> 5) ^ x) + 0xfbb5u;
        case 4: return (x * 8u + 0x4b81u) ^ (x * 3u);
        case 6:
        case 7: return (x + 0xc5d9u) ^ (x >> 1);
        default: return 0;
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    unsigned first = (unsigned)(challenge & 7u);
    unsigned second = (unsigned)((challenge >> 3) & 7u);
    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);
    x = apply6(first, x) ^ 0x47fbu;
    uint32_t values[1] = {apply6(second, x) + 0x13cdu};
    return write_answer(out, cap, values, 1);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t selector = (uint32_t)context & 3u;
    uint32_t folded_challenge = (uint32_t)challenge
                              ^ (uint32_t)(challenge >> 32);
    uint32_t folded_context = (uint32_t)context
                            ^ (uint32_t)(context >> 32);
    uint32_t mixed = ((folded_challenge ^ folded_context) * 7u + 0xefedu)
                   ^ 0x8ad1u;
    uint32_t first = mixed ^ (selector * 0xefedu)
                   ^ (uint32_t)(context >> 16);
    uint32_t second = first ^ (folded_context + selector + 0x8ad1u
                             + (folded_challenge >> 5));

    int size = snprintf(out, cap,
                        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN"
                        "%" PRIu32 " %" PRIu32 " %" PRIu32,
                        selector, first, second);
    return size < 0 || (size_t)size >= cap;
}
