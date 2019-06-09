
  #define MAX_WORDS 100
  #define MAX_WORD_LEN 30


  typedef enum {
     REGISTER,
     REQUEST,
     RESPONSE,
     PING,
     FAILED
  } message_type;

  typedef struct message {
    message_type type;
    int id;
    int counter;
    char text[4096];
    char words[MAX_WORDS][MAX_WORD_LEN];
    int words_counter[MAX_WORDS];
  } message;

