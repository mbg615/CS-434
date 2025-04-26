int main() {
    int x = 0;
    int y = 0;
    while (y != -1) {
        print(x);
        if ((x % 15) == 0) {
            print("foobar");
        } else {
            if ((x % 3) == 0) {
                print("foo");
            }
            if ((x % 5) == 0) {
                print("bar");
            }
        }
        x = x + 1;
        read(y);
    }
    return 56;
}
