void p(char *param_1, char *param_2)
{
    char buffer[4104];

    puts(param_2);
    read(0, buffer, 0x1000); // read 4096 bytes from stdin into buffer
    char *newline_pos = strchr(buffer, '\n'); // find the newline character
    *newline_pos = '\0';
    strncpy(param_1, buffer, 20);
}

void pp(char *dest_buffer)
{
    char first_input[20];
    char second_input[20];
    
    p(first_input, "First prompt");
    
    p(second_input, "Second prompt");
    
    strcpy(dest_buffer, first_input);
    
    int len = strlen(dest_buffer);
    
    dest_buffer[len] = ' ';
    dest_buffer[len + 1] = '\0';
    
    strcat(dest_buffer, second_input);
}


int main()
{
    char final_buffer[54];
    pp(final_buffer);
    puts(final_buffer);

    return 0;
}