#include <iostream>
using namespace std;

static int hit = 0, miss = 0; // ĳ�� hit Ƚ�� �� miss Ƚ��
double c_time = 0.1, m_time = 1, h_time = 3; // ĳ�� ���� �ð�, �޸� ���� �ð�, �ϵ��ũ ���� �ð� 
static double r_time = 0; // �� ���ٽð�

int HardDisk[5000]; // �ϵ��ũ
const int RAM_size = 500, L1_size = 5, L2_size = 20, L3_size = 200; // ��, ĳ�� 3���� ������
const int L1_1_size = 10, L1_2_size = 100; // Ư���� �������� ����� ĳ�� ������

int RAM[RAM_size], L1[L1_size], L2[L2_size], L3[L3_size]; // ����ť�� ������ ���� ĳ�õ�
int L1_1[L1_1_size], L1_2[L1_2_size]; // Ư���� �������� ����ť�� ������ ĳ�õ�
static int front1 = 0, rear1 = 0, front2 = 0, rear2 = 0, front3 = 0, rear3 = 0, frontR = 0, rearR = 0;// ĳ��, �� ����ť�� front�� rear �ε��� ������
// front1�� rear1�� ť�� ���� L1ĳ�ÿ��� fifo��å�� ���ϱ� ���� ���ȴ�
// front2�� rear2�� ť�� ���� L2ĳ�ÿ��� fifo��å�� ���ϱ� ���� ���ȴ�
// front3�� rear3�� ť�� ���� L3ĳ�ÿ��� fifo��å�� ���ϱ� ���� ���ȴ�
// frontR�� rearR�� ť�� ���� ������ fifo��å�� ���ϱ� ���� ���ȴ�

// �Լ� ����
void searchCache(int r);
void searchL1(int r); // L1 ĳ�ÿ��� r�� �����ϴ��� ã�� ���ٸ� ĳ�ÿ� �����͸� �߰��Ѵ�
void searchL2(int r); // L2 ĳ�ÿ��� r�� �����ϴ��� ã�� ���ٸ� ĳ�ÿ� �����͸� �߰��Ѵ�
void searchL3(int r); // L3 ĳ�ÿ��� r�� �����ϴ��� ã�� ���ٸ� ĳ�ÿ� �����͸� �߰��Ѵ�
void searchSmallCache(int r); // (Ư����) ����� ���� L1_1ĳ�ÿ��� �˻�
void searchLargeCache(int r); // (Ư����) ����� ū L1_2ĳ�ÿ��� �˻�
void searchRAM(int r); // ������ r�� �����ϴ��� ã�� ���ٸ� ĳ�ÿ� �����͸� �߰��Ѵ�
void searchHD(int r); // �ϵ��ũ���� �����͸� �����´�

int main() {
    for (int i = 0; i < 5000; i++) {
        HardDisk[i] = i + 1; // 5000���� ������ ����(1~5000) ����
        // ������ ĳ�ÿ� ���� ���� �ʱ�ȭ�� �ʿ� ����
        // c++������ �⺻������ 0���� �ʱ�ȭ�Ǳ� ������ ��ġ�� �ʴ´�
    }

    int r1, r2; // ���� ������ ����� �������� ��
    double miss_rate, hit_rate;   // �̽����� ��Ʈ�� �����ϴ� ���� 
    int n; //���� ���� 2���� �ݺ� Ƚ���� �Է¹��� �� ����ϴ� ����

    // ���� ���� 1
    // ���������� ���� �������� �������� �̶� �ݺ� Ƚ���� 10���̴�.
    cout << "���� ����1: 10�� �ݺ�" << endl;
    for (int i = 0; i < 10; i++) {
        r1 = rand() % 5000 + 1; // R1�� �������� �о��
        r2 = rand() % 5000 + 1; // R2�� �������� �о��

        // L1ĳ�ÿ� ��������1�� ���� �ִ� �� ã�� ������
        cout << "<r1>" << endl;
        searchCache(r1);

        // L1 ĳ�� ���� ����� �����͸� ����Ѵ�
        cout << "L1 cache: ";
        for (int k = 0; k < L1_size; k++) {
            cout << L1[k] << ' ';
        }

        // �� ���� ����� �����͸� ����Ѵ�
        cout << endl << "RAM: ";
        for (int k = 0; k < RAM_size; k++) {
            cout << RAM[k] << ' ';
        }


        // L1ĳ�ÿ� ��������2�� ���� �ִ� �� ã�� ������
        cout << endl << endl << "<r2>" << endl;
        searchCache(r2);

        // L1 ĳ�� ���� ����� �����͸� ����Ѵ�
        cout << "L1 cache: ";
        for (int k = 0; k < L1_size; k++) {
            cout << L1[k] << ' ';
        }
        cout << endl;

        // �� ���� ����� �����͸� ����Ѵ�
        cout << "RAM: ";
        for (int k = 0; k < RAM_size; k++) {
            cout << RAM[k] << ' ';
        }
        cout << endl;

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate�� ���ϱ� ���� �����̴�
        hit_rate = 1 - miss_rate; // ������ ���� miss rate�� ����� hit rate�� ���Ѵ�

        // hit, miss, hit rate, miss rate, access time, r1 + r2�� ����� ����Ѵ�
        cout << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << endl;
        cout << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << "     data access time: " << r_time << endl;
        cout << "r1 + r2 : " << r1 << " + " << r2 << " = " << r1 + r2 << endl << endl << endl;

    }

    // ���� ����2�� ���� �ʱ�ȭ ��Ų��
    for (int i = 0; i < L1_size; i++) {
        L1[i] = 0;
    }
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    frontR = 0, rearR = 0, front1 = 0, rear1 = 0;

    // ���� ���� 2
    // ���������� ���� �������� �������� �̶� �ݺ� Ƚ���� Ű����� �Է¹޴´�.
    cout << endl << "���� ����2: L1�� �̿��� ���� ����" << endl;
    cout << "���� ������ ���� �ݺ� Ƚ���� �Է��ϼ���: ";
    cin >> n;
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1�� �������� �о�� (1~5000)
        r2 = rand() % 5000 + 1; // R2�� �������� �о�� (1~5000)

        searchCache(r1); // L1 ĳ�ø��� �̿��Ͽ� r1�� ã�´�
        searchCache(r2); // L1 ĳ�ø��� �̿��Ͽ� r2�� ã�´�

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate�� ���ϱ� ���� �����̴�
        hit_rate = 1 - miss_rate; // ������ ���� miss rate�� ����� hit rate�� ���Ѵ�

        // 1000�� �ݺ��� ������ ����Ѵ�
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time, r1 + r2�� ����� ����Ѵ�
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // ���� ����2�� ���� �ʱ�ȭ ��Ų��
    for (int i = 0; i < L1_size; i++) {
        L1[i] = 0;
    }
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    frontR = 0, rearR = 0, front1 = 0, rear1 = 0;

    // ���������� ���� �������� �������� �̶� �ݺ� Ƚ���� Ű����� �Է¹޴´�.
    cout << endl << endl << "���� ���� 2: L1~L3�� �̿��� ���� ����";
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1�� �������� �о�� (1~5000)
        r2 = rand() % 5000 + 1; // R2�� �������� �о�� (1~5000)
        searchL1(r1); // r1�� L1 ĳ�ÿ������� ã�Ƴ�����
        searchL1(r2); // r2�� L1 ĳ�ÿ������� ã�Ƴ�����

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate�� ���ϱ� ���� �����̴�
        hit_rate = 1 - miss_rate; // ������ ���� miss rate�� ����� hit rate�� ���Ѵ�

        // 1000�� �ݺ��� ������ ����Ѵ�
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time�� ����Ѵ�
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // Ư���� ���� ������ ���� �ʱ�ȭ
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    // ��������1,2�� �ٸ� ĳ�ø� ��������� ����ť�� ����� ���� front�� rear �ʱ�ȭ
    frontR = 0, rearR = 0, front1 = 0, rear1 = 0, front2 = 0, rear2 = 0; 

    cout << endl << "Ư���� ���� ���� : ĳ�ø� �ϳ��� ����� �� ĳ�� ũ�⿡ ���� ��" << endl << endl;
    cout << "����� ���� L1_1ĳ�� ��� (L1_1_size = 10)" << endl << endl;
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1�� �������� �о�� (1~5000)
        r2 = rand() % 5000 + 1; // R2�� �������� �о�� (1~5000)

        // L1�� ��������1, ��������2�� ���� �ִ� �� ã�� ������
        searchSmallCache(r1);
        searchSmallCache(r2);

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate�� ���ϱ� ���� �����̴�
        hit_rate = 1 - miss_rate; // ������ ���� miss rate�� ����� hit rate�� ���Ѵ�

        // 1000�� �ݺ��� ������ ����Ѵ�
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time�� ����Ѵ�
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // L1ĳ�ø� ����� ��� L1ĳ�ÿ� RAM���� ���
    // L1 ĳ�� ���� ����� �����͸� ����Ѵ�
    cout << "L1_1 cache: ";
    for (int k = 0; k < L1_1_size; k++) {
        cout << L1_1[k] << ' ';
    }
    // �� ���� ����� �����͸� ����Ѵ�
    cout << endl << endl << "RAM: ";
    for (int k = 0; k < RAM_size; k++) {
        cout << RAM[k] << ' ';
    }

    // L2������ ���� ������ ���� �ʱ�ȭ
    for (int i = 0; i < RAM_size; i++) {
        RAM[i] = 0;
    }
    hit = miss = r_time = 0.0;
    frontR = 0, rearR = 0;

    cout << endl << endl << endl << "����� ū L1_2ĳ�� ��� (L1_2_size = 100)" << endl << endl;
    for (int i = 0; i < n; i++) {
        r1 = rand() % 5000 + 1; // R1�� �������� �о�� (1~5000)
        r2 = rand() % 5000 + 1; // R2�� �������� �о�� (1~5000)

        // L1_2�� ��������1, ��������2�� ���� �ִ� �� ã�� ������
        searchLargeCache(r1);
        searchLargeCache(r2);

        miss_rate = (double)miss / ((double)hit + (double)miss); // miss rate�� ���ϱ� ���� �����̴�
        hit_rate = 1 - miss_rate; // ������ ���� miss rate�� ����� hit rate�� ���Ѵ�

        // 1000�� �ݺ��� ������ ����Ѵ�
        if (i % 1000 == 0) {
            // hit, miss, hit rate, miss rate, access time�� ����Ѵ�
            cout << endl << "times repeated: " << i + 1 << "     hit: " << hit << "    " << " miss: " << miss << "    " << " data access time: " << r_time;
            cout << endl << "hit_rate: " << hit_rate << "     miss_rate: " << miss_rate << endl << endl;
        }
    }

    // L1_2ĳ�ø� ����� ��� L1_2ĳ�ÿ� RAM���� ���
    cout << "L1_2 cache: ";
    for (int k = 0; k < L1_2_size; k++) {
        cout << L1_2[k] << ' ';
    }
    cout << endl << endl << "RAM (L2������): ";
    for (int k = 0; k < RAM_size; k++) {
        cout << RAM[k] << ' ';
    }

    return 0;
}

// ĳ�ð� L1�� ���� �� �����͸� ã�´�
void searchCache(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += c_time; // �� access time�� ���ϱ� ���� cache access time�� ���Ѵ�

    // L1 cache�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < L1_size; i++) {
        if (L1[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ĳ�ÿ� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rear1 + 1) % L1_size == front1) {
            front1 = (front1 + 1) % L1_size;
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        searchRAM(r);
    }
}

// L1���� ���� ã�´�
void searchL1(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += c_time; // �� access time�� ���ϱ� ���� cache access time�� ���Ѵ�

    // L1 cache�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < L1_size; i++) {
        if (L1[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ĳ�ÿ� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rear1 + 1) % L1_size == front1) {
            front1 = (front1 + 1) % L1_size;
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            L1[rear1] = r;
            rear1 = (rear1 + 1) % L1_size;
        }
        searchL2(r);
    }
}

// L2 ĳ�ÿ��� �����͸� ã�´� ���� �����Ͱ� ���ٸ� L3 ĳ�ÿ��� ã�´�
void searchL2(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += c_time; // �� access time�� ���ϱ� ���� cache access time�� ���Ѵ�

    // L2 cache�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < L2_size; i++) {
        if (L2[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ĳ�ÿ� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rear2 + 1) % L2_size == front2) {
            front2 = (front2 + 1) % L2_size;
            L2[rear2] = r;
            rear2 = (rear2 + 1) % L2_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            L2[rear2] = r;
            rear2 = (rear2 + 1) % L2_size;
        }
        searchL3(r);
    }

}

// L3 ĳ�ÿ��� �����͸� ã�´� ���� �����Ͱ� ���ٸ� ������ ã�´�
void searchL3(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += c_time;  // �� access time�� ���ϱ� ���� cache access time�� ���Ѵ�

    // L3 cache�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < L3_size; i++) {
        if (L3[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ĳ�ÿ� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rear3 + 1) % L3_size == front3) {
            front3 = (front3 + 1) % L3_size;
            L3[rear3] = r;
            rear3 = (rear3 + 1) % L3_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            L3[rear3] = r;
            rear3 = (rear3 + 1) % L3_size;
        }

        searchRAM(r);
    }

}

// (Ư����) ����� ���� L1_1ĳ�ÿ��� �˻�
void searchSmallCache(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += c_time; //�� access time�� ���ϱ� ���� cache access time�� ���Ѵ�

    // L1_1 cache�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < L1_1_size; i++) {
        if (L1_1[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ĳ�ÿ� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rear1 + 1) % L1_1_size == front1) {
            front1 = (front1 + 1) % L1_1_size;
            L1_1[rear1] = r;
            rear1 = (rear1 + 1) % L1_1_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            L1_1[rear1] = r;
            rear1 = (rear1 + 1) % L1_1_size;
        }
        searchRAM(r);
    }
}

// (Ư����) ����� ū L1_2ĳ�ÿ��� �˻�
void searchLargeCache(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += c_time; //�� access time�� ���ϱ� ���� cache access time�� ���Ѵ�

    // L1_2 cache�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < L1_2_size; i++) {
        if (L1_2[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ĳ�ÿ� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rear2 + 1) % L1_2_size == front2) {
            front2 = (front2 + 1) % L1_2_size;
            L1_2[rear2] = r;
            rear2 = (rear2 + 1) % L1_2_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            L1_2[rear2] = r;
            rear2 = (rear2 + 1) % L1_2_size;
        }
        searchRAM(r);
    }
}

//���� �����Ͱ� �����ϴ��� �˻��Ѵ� ���� ���ٸ� searchHD�� ���� �ϵ��ũ�� �����Ͽ� �����´�
void searchRAM(int r) {
    bool valid = 0; // valid�� ���� �ִٸ� true�� �ٲ��
    r_time += m_time; // �� access time�� ���ϱ� ���� ram access time�� ���Ѵ�

    // RAM�� ã���� �ϴ� ���� �ִٸ� hit�� �ϳ� ������Ű�� �ִٴ� ǥ�÷� valid�� �߰��Ѵ�.
    for (int i = 0; i < RAM_size; i++) {
        if (RAM[i] == r) {
            hit++;
            valid = 1;
            break;
        }
    }

    // ���� ���� ���ٸ� miss�� �ϳ� ������Ű�� ����ť�� �ִ´�
    if (valid == 0) {
        miss++;

        // ť�� �� á�ٸ� fifo�� �����Ų��.
        if ((rearR + 1) % RAM_size == frontR) {
            frontR = (frontR + 1) % RAM_size;
            RAM[rearR] = r;
            rearR = (rearR + 1) % RAM_size;
        }
        // ť�� �� ���� �ʾҴٸ� ������� �Է��Ѵ�
        else {
            RAM[rearR] = r;
            rearR = (rearR + 1) % RAM_size;
        }

        searchHD(r);
    }

}

// ĳ�ó� ���� ���� ���ٸ� �ϵ��ũ�� ������ �����͸� ��������
void searchHD(int r) {
    r_time += h_time; // �� access time�� ���ϱ� ���� hard disk access time�� ���Ѵ�
}