/*             N = 3

[0]-----[1]-----[2]-----[3]-----[4]
 |     / |     / |     / |     / |
 |   /   |   /   |   /   |   /   |
 | /     | /     | /     | /     |
[5]-----[6]-----[7]-----[8]-----[9]
 |     / |     / |     / |     / |
 |   /   |   /   |   /   |   /   |
 | /     | /     | /     | /     |
[10]----[11]----[12]----[13]----[14]
 |     / |     / |     / |     / |
 |   /   |   /   |   /   |   /   |
 | /     | /     | /     | /     |
[15]----[16]----[17]----[18]----[19]
 |     / |     / |     / |     / |
 |   /   |   /   |   /   |   /   |
 | /     | /     | /     | /     |
[20]----[21]----[22]----[23]----[24]
*/
constexpr int N = 3;
constexpr int ans[4*4*6] = {
     0, 5, 1,   1, 5, 6,
     1, 6, 2,   2, 6, 7,
     2, 7, 3,   3, 7, 8,
     3, 8, 4,   4, 8, 9,

     5,10, 6,   6,10,11,
     6,11, 7,   7,11,12,
     7,12, 8,   8,12,13,
     8,13, 9,   9,13,14,

    10,15,11,  11,15,16,
    11,16,12,  12,16,17,
    12,17,13,  13,17,18,
    13,18,14,  14,18,19,

    15,20,16,  16,20,21,
    16,21,17,  17,21,22,
    17,22,18,  18,22,23,
    18,23,19,  19,23,24,
};

#include <iostream>
#include <iomanip>

void print3(const int* faces, int i) {
    for (int n = i; n < i+3; n++) {
        if (faces[n]==ans[n])
            std::cout << " " << std::right << std::setw(2) << faces[n] << " ";
        else
            std::cout << "[" << std::right << std::setw(2) << faces[n] << "]";

        std::cout << ' ';
    }
}

int main() {
    constexpr int FACES_SIZE = 2*(N+1)*(N+1) * 3;
    int faces[FACES_SIZE];
    int i6 = 0;
    for(int i=0, x = 0; x < N+1; x++) {
        for (int y = 0; y < N+1; y++, i++) {
            faces[i6++] = i;
            faces[i6++] = i + N+2;
            faces[i6++] = i + 1;

            faces[i6++] = i + 1;
            faces[i6++] = i + N+2;
            faces[i6++] = i + N+2 + 1;
        }
        i++;
    }

    for (int i = 0; i < FACES_SIZE; i++) {
        if (faces[i] != ans[i]) {
            i = 0;
            for (int x = 0; x < FACES_SIZE/6; x++, i+=6) {
                print3(faces, i);
                std::cout << "  ";
                print3(faces, i+3);
                std::cout << '\n';
                if (x%4==3) std::cout << '\n';
            }
            break;
        }
    }

    return 0;
}
