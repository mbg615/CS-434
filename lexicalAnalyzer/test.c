void fun(int z) {
    z = z / 3;
    return;
}

int main() {
    int x = 13;
    int y = -24;

    while(1) {
        if (x + y < 0) {
            x = x + 1;
            break;
        }
    }
    return 0;
}
