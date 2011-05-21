#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <cstdio>

using namespace Gecode;

int N;

class SquarePacking : public Script {
    protected:
        IntVar s;
        IntVarArray x;
        IntVarArray y;
        // Step 1a: Create static size method
        static int size(int square_number) {
            return N - square_number;
        }

    public:
        SquarePacking(const SizeOptions& opt) 
                : x(*this, opt.size()), y(*this, opt.size()) {
            // Step 1b: Initialize variables s, x, y
            int min = 0;
            int max = 0;
            for (int i = 0; i < opt.size(); ++i) {
                min += i*i;
                max += i;
            }
            s = IntVar(*this, sqrt(min), max);

            for (int i = 0; i < opt.size(); ++i) {
                x[i] = IntVar(*this, 0, max);
                y[i] = IntVar(*this, 0, max);
            }

            for (int i = 0; i < N; ++i) {
                rel(*this, (x[i] + size(i)) <= s);
                rel(*this, (y[i] + size(i)) <= s);
            }

            // Step 2: No overlapping between squares, expressed with
            // reification.
            for (int i = 0; i < N-1; i++) {
               for (int j = i+1; j < N-1; j++) {
                   rel(*this, 
                        (x[i] + size(i) <= x[j]) || 
                        (x[j] + size(j) <= x[i]) ||
                        (y[i] + size(i) <= y[j]) || 
                        (y[j] + size(j) <= y[i]));  
                }
            }

            // Step 3: Sum of square sizes for every row and column less than
            // s, expressed with reification.
            IntArgs sizes(N-1);
            for (int i = 0; i < N-1; ++i) {
                sizes[i] = size(i);
            }

            for (int col = 0; col < s.max(); ++col) {
                BoolVarArgs bx(*this, N-1, 0, 1);
                for (int i = 0; i < N-1; ++i) {
                    dom(*this, y[i], col - size(i) + 1, col, bx[i]);
                }
                linear(*this, sizes, bx, IRT_LQ, s);
            }

            for (int col = 0; col < s.max(); ++col) {
                BoolVarArgs bx(*this, N-1, 0, 1);
                for (int i = 0; i < N-1; ++i) {
                    dom(*this, x[i], col - size(i) + 1, col, bx[i]);
                }
                linear(*this, sizes, bx, IRT_LQ, s);
             }

            // Step 4.1: Problem decomposition 
            rel(*this, (s*s) > ((N * (N+1) * (2 * N+1))/6));

            // Step 4.2: Symmetry removal
            rel(*this, x[0] <= ((s - N)/2));
            rel(*this, y[0] <= ((s - N)/2));

            // Step 4.3: Empty strip dominance
            for (int i = 0; i < N; ++i) {
                int siz = size(i);
                int gt = 0;
                if (siz == 2 || siz == 4)
                    gt = 2;
                else if (siz == 3 || (siz >= 5 && siz <= 8))
                    gt = 3;
                else if (siz <= 11)
                    gt = 4;
                else if (siz <= 17)
                    gt = 5;
                else if (siz <= 21)
                    gt = 6;
                else if (siz <= 29)
                    gt = 7;
                else if (siz <= 34)
                    gt = 8;
                else if (siz <= 44)
                    gt = 9;
                else if (siz <= 45)
                    gt = 10;

                if (gt != 0) {
                    rel(*this, x[i] != gt);
                    rel(*this, y[i] != gt);
                }
            }

            // Step 4.4: Ignoring size 1 squares, this is done by only looping
            // until N-1 instead of N

            // Step 5: Branching heuristics
            branch(*this, s, INT_VAL_MIN); // (5a) Branch on s first
            // (5c) INT_VAR_NONE means going through the array in order, that
            // is from the biggest to the smallest square
            // (5d) INT_VAL_MIN chooses the minimum value, that is placing it
            // from left to right and top to botom.
            // (5b) First assign x-coordinates.     
            branch(*this, x, INT_VAR_NONE, INT_VAL_MIN);
            // (5b) then y-coordinates. 
            branch(*this, y, INT_VAR_NONE, INT_VAL_MIN);
        }

        SquarePacking(bool share, SquarePacking& sp) : Script(share, sp) {
            x.update(*this, share, sp.x);
            y.update(*this, share, sp.y);
            s.update(*this, share, sp.s);
        }

        virtual Space* copy(bool share) {
            return new SquarePacking(share, *this);
        }

        virtual void print(std::ostream& os) const {
            char output[s.val()+1][s.val()+1];
            memset(output, 0, sizeof(output));

            for (int i = 0; i < N-1; ++i) {
                for (int j = 0; j < size(i); ++j) {
                    for (int k = 0; k < size(i); ++k) {
                        output[x[i].val()+j][y[i].val()+k] = size(i);
                    }
                }
            }

            bool one = false;
            for (int i = 0; i < s.val(); ++i) {
                for (int j = 0; j < s.val(); ++j) {
                    if (output[i][j] == 0) {
                        if (!one) {
                            one = true;
                            printf(" 1");
                        } else {
                            printf("  ");
                        }
                    } else {
                        printf("%2d", output[i][j]);
                    }
                }
                printf("\n");
            }
        }
};

int main(int argc, char* argv[]) {
    SizeOptions opt("SquarePacking");
    opt.size(10);
    opt.parse(argc,argv);
    N = opt.size();
    Script::run<SquarePacking, DFS, SizeOptions>(opt);
    return 0;
}
