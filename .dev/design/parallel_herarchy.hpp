
struct B
{
    virtual void print();
};

struct A
{
    B;
};

///////////////////////

struct B_string
{
    string x;
    void print() override;
};

struct A_string : A
{
};

///////////////////////

struct B_int
{
    int x;
    void print() override;
};

struct A_int : A
{
};

///////////////////////
// SOLUTION
///////////////////////

template <class T>
class A
{
    FORCE_SUBCLASS (T --> B)
};

struct A_string : A<string>
{
};

struct A_int : A<int>
{
};
