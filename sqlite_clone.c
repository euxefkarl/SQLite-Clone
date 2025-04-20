#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() { printf("db > "); }

//windows getline implementation since it is not available in the standard library
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  if (lineptr == NULL || n == NULL || stream == NULL) {
      return -1;
  }

  char *buf = *lineptr;
  size_t size = *n;
  int c;
  size_t len = 0;

  if (buf == NULL || size == 0) {
      size = 128; // Initial buffer size
      buf = malloc(size);
      if (buf == NULL) {
          return -1;
      }
  }

  while ((c = fgetc(stream)) != EOF) {
      if (len + 1 >= size) {
          size *= 2; // Double the buffer size
          char *new_buf = realloc(buf, size);
          if (new_buf == NULL) {
              free(buf);
              return -1;
          }
          buf = new_buf;
      }
      buf[len++] = c;
      if (c == '\n') {
          break;
      }
  }

  if (len == 0 && c == EOF) {
      return -1; // No input
  }

  buf[len] = '\0';
  *lineptr = buf;
  *n = size;

  return len;
}

void read_input(InputBuffer* input_buffer) {
  
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

typedef enum{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
}MetaCommandResult;

typedef enum{
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_STATEMENT
}PrepareResult;

MetaCommandResult do_meta_command(InputBuffer* input_buffer){
    if(strcmp(input_buffer->buffer,'.exit') == 0){
      exit(EXIT_SUCCESS);
    }else{
      return META_COMMAND_UNRECOGNIZED_COMMAND;
    }

}

typedef enum{
  STATEMENT_INSERT,
  STATEMENT_SELECT
}StatementType;

typedef struct {
  StatementType type;
}Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){ 
    if(strcmp(input_buffer->buffer,'insert',6)==0){
      statement->type = STATEMENT_INSERT;
      return PREPARE_SUCCESS;
    }
    if(strcmp(input_buffer->buffer,'select')==0){
      statement->type = STATEMENT_SELECT;
      return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement){
    switch(statement->type){ 
      case(STATEMENT_INSERT):
        printf("This is where we would do an insert.\n");
        break;
      case(STATEMENT_SELECT):
        printf("This is where we would do a select.\n");
        break;
    }
}
int main(int argc, char* argv[]) {
  InputBuffer* input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    read_input(input_buffer);

    if (strcmp(input_buffer->buffer, ".exit") == 0) {
      if(input_buffer->buffer[0]=='.'){
        switch (do_meta_command(input_buffer)){
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'. \n", input_buffer->buffer);
          continue;
        }
      }
      Statement statement;
      switch(prepare_statement(input_buffer, &statement)){
        case (PREPARE_SUCCESS):
          break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
          printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
          continue;
      }
    } 
    execute_statement(&statement);
    printf("Executed. \n");
  }
}