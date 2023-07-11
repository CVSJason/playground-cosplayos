extern "C" void apiPutCharUtf32(int chara);

int main() {
    char a[100];

    a[10] = 'A';
    apiPutCharUtf32(a[10]);

    a[102] = 'A';
    apiPutCharUtf32(a[102]);

    a[114514] = 'A';
    apiPutCharUtf32(a[114514]);

    return 0;
}