int simple_function(int i) {
    if (i > 0) {
        int value = i;
        value += 3;
        value = value * 2;
        return value;
    } else {
        return i * -5 + i;
    }
}
