#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class GameOfLife : public Script {
    protected:
        BoolVarArray cells;
        int n;

    public:
        GameOfLife(const SizeOptions& opt) : cells(*this, (opt.size() + 4) * (opt.size() + 4), 0, 1) {
            // Setup
            n = opt.size() + 4;
            Matrix<BoolVarArgs> matrix(cells, n, n);

            // Constraints
            rel(*this, matrix.row(0), IRT_EQ, 0);
            rel(*this, matrix.col(0), IRT_EQ, 0);
            rel(*this, matrix.row(1), IRT_EQ, 0);
            rel(*this, matrix.col(1), IRT_EQ, 0);
            rel(*this, matrix.row(n-1), IRT_EQ, 0);
            rel(*this, matrix.col(n-1), IRT_EQ, 0);
            rel(*this, matrix.row(n-2), IRT_EQ, 0);
            rel(*this, matrix.col(n-2), IRT_EQ, 0);

            for (int i = 2; i < n-2; ++i) {
                for (int j = 2; j < n-2; ++j) {
                    BoolVar stillAlive, newLife;
                    BoolExpr exp = ((matrix(i-1,j-1) + matrix(i,j-1) + matrix(i+1,j-1) 
                            + matrix(i-1,j)    + matrix(i,j)   + matrix(i+1,j)
                            + matrix(i-1,j+1)  + matrix(i,j+1) + matrix(i+1,j+ 1) == 2 && matrix(i,j)) || 

                            (matrix(i-1,j-1) + matrix(i,j-1) + matrix(i+1,j-1) 
                            + matrix(i-1,j)    + matrix(i,j)   + matrix(i+1,j)
                            + matrix(i-1,j+1)  + matrix(i,j+1) + matrix(i+1,j+ 1) == 3) );

                            exp.rel(*this, ICL_VAL);

                    // rel(*this, matrix(i-1,j-1) + matrix(i,j-1) + matrix(i+1,j-1) 
                            // + matrix(i-1,j)    + matrix(i,j)   + matrix(i+1,j)
                            // + matrix(i-1,j+1)  + matrix(i,j+1) + matrix(i+1,j+ 1) == 2 && matrix(i,j), stillAlive);
// 
                    // rel(*this, matrix(i-1,j-1) + matrix(i,j-1) + matrix(i+1,j-1) 
                            // + matrix(i-1,j)    + matrix(i,j)   + matrix(i+1,j)
                            // + matrix(i-1,j+1)  + matrix(i,j+1) + matrix(i+1,j+ 1) == 3, newLife);


                    rel(*this, matrix(i, j), BOT_OR, newLife, stillAlive);
                }
            }

            // Branch
            branch(*this, cells, INT_VAR_NONE, INT_VAL_MAX);
        }

        GameOfLife(bool share, GameOfLife& sp) : Script(share, sp) {
            cells.update(*this, share, sp.cells);
        }

        virtual Space* copy(bool share) {
            return new GameOfLife(share, *this);
        }

        virtual void print(std::ostream& os) const {
            os << "Game of Life cells" << std::endl;
            Matrix<BoolVarArgs> matrix(cells, n, n);
            for (int i = 2; i < n - 2; ++i) {
                for (int j = 2; j < n - 2; ++j) {
                    if (matrix(i, j).assigned()) {
                        if (matrix(i, j).val() == 1) {
                            os << "X";
                        } else {
                            os << " ";
                        }
                    }
                }
                os << std::endl;
            }
        }
};

int main(int argc, char* argv[]) {
    SizeOptions opt("GameOfLife");
    opt.size(5);
    opt.parse(argc,argv);
    Script::run<GameOfLife, DFS, SizeOptions>(opt);
    return 0;
}
