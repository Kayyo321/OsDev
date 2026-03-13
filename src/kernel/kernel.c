int len_of(char *s) {
    int i;
    for (i = 0; s && *s != 0; ++i) {
        ++s;
    }

    return i;
}

void entry(void) {
    char *video_memory = (char*)0xb8000;

    char *out = "This OS says that wahid a bitch";
    int len = len_of(out);
    for (int i = 0; i < len; ++i) {
        video_memory[i * 2] = out[i];
        video_memory[i * 2 + 1] = 0x0F;
    }
}