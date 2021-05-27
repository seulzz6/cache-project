#include <iostream>
using namespace std;

static int hit = 0, miss = 0; // 캐시 hit 횟수 및 miss 횟수
double c_time = 0.1, m_time = 1, h_time = 3; // 캐시 접근 시간, 메모리 접근 시간, 하드디스크 접근 시간 
static double r_time = 0; // 총 접근시간

int HardDisk[5000]; // 하드디스크
const int RAM_size = 500, L1_size = 5, L2_size = 20, L3_size = 200; // 램, 캐시 3종류 사이즈
const int L1_1_size = 10, L1_2_size = 100; // 특이점 구현에서 사용할 캐시 사이즈

int RAM[RAM_size], L1[L1_size], L2[L2_size], L3[L3_size]; // 원형큐로 구현할 램과 캐시들
int L1_1[L1_1_size], L1_2[L1_2_size]; // 특이점 구현에서 원형큐로 구현할 캐시들
static int front1 = 0, rear1 = 0, front2 = 0, rear2 = 0, front3 = 0, rear3 = 0, frontR = 0, rearR = 0;// 캐시, 램 원형큐의 front와 rear 인덱스 포인터
// front1과 rear1은 큐를 통해 L1캐시에서 fifo정책을 행하기 위해 사용된다
// front2과 rear2은 큐를 통해 L2캐시에서 fifo정책을 행하기 위해 사용된다
// front3과 rear3은 큐를 통해 L3캐시에서 fifo정책을 행하기 위해 사용된다
// frontR과 rearR은 큐를 통해 램에서 fifo정책을 행하기 위해 사용된다

// 함수 선언
void searchCache(int r);
void searchL1(int r); // L1 캐시에서 r이 존재하는지 찾고 없다면 캐시에 데이터를 추가한다
void searchL2(int r); // L2 캐시에서 r이 존재하는지 찾고 없다면 캐시에 데이터를 추가한다
void searchL3(int r); // L3 캐시에서 r이 존재하는지 찾고 없다면 캐시에 데이터를 추가한다
void searchSmallCache(int r); // (특이점) 사이즈가 작은 L1_1캐시에서 검색
void searchLargeCache(int r); // (특이점) 사이즈가 큰 L1_2캐시에서 검색
void searchRAM(int r); // 램에서 r이 존재하는지 찾고 없다면 캐시에 데이터를 추가한다
void searchHD(int r); // 하드디스크에서 데이터를 가져온다

int main() {
    for (int i = 0; i < 5000; i++) {
        HardDisk[i] = i + 1; // 5000개의 유일한 정수(1~5000) 저장
        // 나머지 캐시와 램은 따로 초기화할 필요 없다
        // c++에서는 기본적으로 0으로 초기화되기 때문에 겹치지 않는다
    }

    int r1, r2; // 성능 측정시 사용할 레지스터 값
    double miss_rate, hit_rate;   // 미스율과 히트율 저장하는 변수 
    int n; //성능 측정 2에서 반복 횟수를 입력받을 때 사용하는 변수

    // 성능 측정 1
    // 레지스터의 값을 랜덤으로 가져오며 이때 반복 횟수는 10번이다.
    cout << "성능 측정1: 10번 반복" << endl;
    for (int i = 0; i < 10; i++) {
        r1 = rand() % 5000 + 1; // R1값 랜덤으로 읽어옴
        r2 = rand() % 5000 + 1; // R2값 랜덤으로 읽어옴

        // L1캐시에 레지스터1의 값이 있는 지 찾아 나간다
        cout << "<r1>" << endl;
        searchCache(r1);

        // L1 캐시 내에 저장된 데이터를 출력한다
        cout << "L1 cache: ";
        for (int k = 0; k < L1_size; k++) {
            cout << L1[k] << ' ';
        }

        // 램 내에 저장된 데이터를 출력한다
        cout << endl << "RAM: ";
        for (int k = 0; k < RAM_size; k++) {
            cout << RAM[k] << ' ';
        }


        // L1캐시에 레지스터2의 값이 있는 지 찾아 나간다
        cout << endl << endl << "<r2>" << endl;
        searchCache(r2);

        // L1 캐시 내에 저장된 데이터를 출력한다
        cout << "L1 cache: ";
        for (int k = 0; k < L1_size; k++) {
            cout << L1[k] << ' ';
        }
        cout << endl;

        // 램 내에 저장된 데이터를 출력한다
        cout << "RAM: ";
        for (int k = 0; k < RAM_size; k++) {
            cout << RAM[k] << ' ';
        }
        cout << endl;

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate을 구하기 위한 수식이다
        hit_rate = 1 - miss_rate; // 위에서 구한 miss rate을 사용해 hit rate을 구한다

        // hit, miss, hit rate, miss rate, access time, r1 + r2의 결과를 출력한다
        cout << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << endl;
        cout << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << "     data access time: " << r_time << endl;
        cout << "r1 + r2 : " << r1 << " + " << r2 << " = " << r1 + r2 << endl << endl << endl;

    }

    // 성능 측정2를 위해 초기화 시킨다
    for (int i = 0; i < L1_size; i++) {
        L1[i] = 0;
    }
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    frontR = 0, rearR = 0, front1 = 0, rear1 = 0;

    // 성능 측정 2
    // 레지스터의 값을 랜덤으로 가져오며 이때 반복 횟수는 키보드로 입력받는다.
    cout << endl << "성능 측정2: L1을 이용한 성능 측정" << endl;
    cout << "성능 측정을 위해 반복 횟수를 입력하세요: ";
    cin >> n;
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1값 랜덤으로 읽어옴 (1~5000)
        r2 = rand() % 5000 + 1; // R2값 랜덤으로 읽어옴 (1~5000)

        searchCache(r1); // L1 캐시만을 이용하여 r1을 찾는다
        searchCache(r2); // L1 캐시만을 이용하여 r2를 찾는다

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate을 구하기 위한 수식이다
        hit_rate = 1 - miss_rate; // 위에서 구한 miss rate을 사용해 hit rate을 구한다

        // 1000번 반복할 때마다 출력한다
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time, r1 + r2의 결과를 출력한다
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // 성능 측정2를 위해 초기화 시킨다
    for (int i = 0; i < L1_size; i++) {
        L1[i] = 0;
    }
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    frontR = 0, rearR = 0, front1 = 0, rear1 = 0;

    // 레지스터의 값을 랜덤으로 가져오며 이때 반복 횟수는 키보드로 입력받는다.
    cout << endl << endl << "성능 측정 2: L1~L3를 이용한 성능 측정";
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1값 랜덤으로 읽어옴 (1~5000)
        r2 = rand() % 5000 + 1; // R2값 랜덤으로 읽어옴 (1~5000)
        searchL1(r1); // r1을 L1 캐시에서부터 찾아나간다
        searchL1(r2); // r2을 L1 캐시에서부터 찾아나간다

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate을 구하기 위한 수식이다
        hit_rate = 1 - miss_rate; // 위에서 구한 miss rate을 사용해 hit rate을 구한다

        // 1000번 반복할 때마다 출력한다
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time을 출력한다
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // 특이점 성능 측정을 위한 초기화
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    // 성능측정1,2와 다른 캐시를 사용하지만 원형큐의 사용을 위해 front와 rear 초기화
    frontR = 0, rearR = 0, front1 = 0, rear1 = 0, front2 = 0, rear2 = 0; 

    cout << endl << "특이점 성능 측정 : 캐시를 하나만 사용할 때 캐시 크기에 따른 비교" << endl << endl;
    cout << "사이즈가 작은 L1_1캐시 사용 (L1_1_size = 10)" << endl << endl;
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1값 랜덤으로 읽어옴 (1~5000)
        r2 = rand() % 5000 + 1; // R2값 랜덤으로 읽어옴 (1~5000)

        // L1에 레지스터1, 레지스터2의 값이 있는 지 찾아 나간다
        searchSmallCache(r1);
        searchSmallCache(r2);

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate을 구하기 위한 수식이다
        hit_rate = 1 - miss_rate; // 위에서 구한 miss rate을 사용해 hit rate을 구한다

        // 1000번 반복할 때마다 출력한다
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time을 출력한다
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // L1캐시만 사용한 경우 L1캐시와 RAM내용 출력
    // L1 캐시 내에 저장된 데이터를 출력한다
    cout << "L1_1 cache: ";
    for (int k = 0; k < L1_1_size; k++) {
        cout << L1_1[k] << ' ';
    }
    // 램 내에 저장된 데이터를 출력한다
    cout << endl << endl << "RAM: ";
    for (int k = 0; k < RAM_size; k++) {
        cout << RAM[k] << ' ';
    }

    // L2에서의 성능 측정을 위한 초기화
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    frontR = 0, rearR = 0;

    cout << endl << endl << endl << "사이즈가 큰 L1_2캐시 사용 (L1_2_size = 100)" << endl << endl;
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1값 랜덤으로 읽어옴 (1~5000)
        r2 = rand() % 5000 + 1; // R2값 랜덤으로 읽어옴 (1~5000)

        // L1_2에 레지스터1, 레지스터2의 값이 있는 지 찾아 나간다
        searchLargeCache(r1);
        searchLargeCache(r2);

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate을 구하기 위한 수식이다
        hit_rate = 1 - miss_rate; // 위에서 구한 miss rate을 사용해 hit rate을 구한다

        // 1000번 반복할 때마다 출력한다
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time을 출력한다
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // L1_2캐시만 사용한 경우 L1_2캐시와 RAM내용 출력
    cout << "L1_2 cache: ";
    for (int k = 0; k < L1_2_size; k++) {
        cout << L1_2[k] << ' ';
    }
    cout << endl << endl << "RAM (L2실행결과): ";
    for (int k = 0; k < RAM_size; k++) {
        cout << RAM[k] << ' ';
    }

    return 0;
}

// 캐시가 L1만 있을 때 데이터를 찾는다
void searchCache(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += c_time; // 총 access time을 구하기 위해 cache access time을 더한다

    // L1 cache에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < L1_size; i++) {
        if (L1[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 캐시에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rear1 + 1) % L1_size == front1) {
            front1 = (front1 + 1) % L1_size;
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        searchRAM(r);
    }
}

// L1에서 값을 찾는다
void searchL1(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += c_time; // 총 access time을 구하기 위해 cache access time을 더한다

    // L1 cache에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < L1_size; i++) {
        if (L1[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 캐시에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rear1 + 1) % L1_size == front1) {
            front1 = (front1 + 1) % L1_size;
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        searchL2(r);
    }
}

// L2 캐시에서 데이터를 찾는다 만일 데이터가 없다면 L3 캐시에서 찾는다
void searchL2(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += c_time; // 총 access time을 구하기 위해 cache access time을 더한다

    // L2 cache에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < L2_size; i++) {
        if (L2[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 캐시에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rear2 + 1) % L2_size == front2) {
            front2 = (front2 + 1) % L2_size;
            L2[rear2] = r;
            rear2 = (rear2 + 1) % L2_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            L2[rear2] = r;
            rear2 = (rear2 + 1) % L2_size;
        }
        searchL3(r);
    }

}

// L3 캐시에서 데이터를 찾는다 만일 데이터가 없다면 램에서 찾는다
void searchL3(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += c_time;  // 총 access time을 구하기 위해 cache access time을 더한다

    // L3 cache에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < L3_size; i++) {
        if (L3[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 캐시에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rear3 + 1) % L3_size == front3) {
            front3 = (front3 + 1) % L3_size;
            L3[rear3] = r;
            rear3 = (rear3 + 1) % L3_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            L3[rear3] = r;
            rear3 = (rear3 + 1) % L3_size;
        }

        searchRAM(r);
    }

}

// (특이점) 사이즈가 작은 L1_1캐시에서 검색
void searchSmallCache(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += c_time; //총 access time을 구하기 위해 cache access time을 더한다

    // L1_1 cache에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < L1_1_size; i++) {
        if (L1_1[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 캐시에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rear1 + 1) % L1_1_size == front1) {
            front1 = (front1 + 1) % L1_1_size;
            L1_1[rear1] = r;
            rear1 = (rear1 + 1) % L1_1_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            L1_1[rear1] = r;
            rear1 = (rear1 + 1) % L1_1_size;
        }
        searchRAM(r);
    }
}

// (특이점) 사이즈가 큰 L1_2캐시에서 검색
void searchLargeCache(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += c_time; //총 access time을 구하기 위해 cache access time을 더한다

    // L1_2 cache에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < L1_2_size; i++) {
        if (L1_2[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 캐시에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rear2 + 1) % L1_2_size == front2) {
            front2 = (front2 + 1) % L1_2_size;
            L1_2[rear2] = r;
            rear2 = (rear2 + 1) % L1_2_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            L1_2[rear2] = r;
            rear2 = (rear2 + 1) % L1_2_size;
        }
        searchRAM(r);
    }
}

//램에 데이터가 존재하는지 검색한다 만일 없다면 searchHD를 통해 하드디스크에 접근하여 가져온다
void searchRAM(int r) {
    bool valid = 0; // valid는 값이 있다면 true로 바뀐다
    r_time += m_time; // 총 access time을 구하기 위해 ram access time을 더한다

    // RAM에 찾고자 하는 값이 있다면 hit를 하나 증가시키고 있다는 표시로 valid를 추가한다.
    for (int i = 0; i < RAM_size; i++) {
        if (RAM[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // 램에 값이 없다면 miss를 하나 증가시키고 원형큐에 넣는다
    if (valid == 0) {
        miss++;

        // 큐가 다 찼다면 fifo를 적용시킨다.
        if ((rearR + 1) % RAM_size == frontR) {
            frontR = (frontR + 1) % RAM_size;
            RAM[rearR] = r;
            rearR = (rearR + 1) % RAM_size;
        }
        // 큐가 다 차지 않았다면 순서대로 입력한다
        else {
            RAM[rearR] = r;
            rearR = (rearR + 1) % RAM_size;
        }

        searchHD(r);
    }

}

// 캐시나 램에 값이 없다면 하드디스크에 접근해 데이터를 가져간다
void searchHD(int r) {
    r_time += h_time; // 총 access time을 구하기 위해 hard disk access time을 더한다
}