struct basic {
  int a;
  int b;
  int c;
  int d;
};

struct middle_padding {
  char c;
  int a;
};

struct end_padding {
  int a;
  char c;
};

struct bitfield {
  int a : 5;
  int b : 6;
  int c : 21;
};

struct struct_members {
  struct basic a;
  struct basic b;
};

struct array_members {
  int a[3];
  int b;
};

union basic_union {
  char c;
  short s;
  int i;
  long long l;
};

#ifdef __cplusplus
class Class {
public:
  int a;
  int b;
};
#endif // __cplusplus
