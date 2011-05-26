#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class GameOfLife : public Script {
    protected:
        BoolVarArray cells;
        IntVarArray minicells;

    public:
        GameOfLife(const SizeOptions& opt) : 
                cells(*this, (opt.size() + 4) * (opt.size() + 4), 0, 1),
                minicells(*this,pow(ceil(opt.size()/3.0),2),0,6) {
            // Setup
            int n = opt.size() + 4;
            Matrix<BoolVarArgs> matrix(cells, n, n);

            // Constraints, 2 cells wide border
            rel(*this, matrix.row(0), IRT_EQ, 0);
            rel(*this, matrix.col(0), IRT_EQ, 0);
            rel(*this, matrix.row(1), IRT_EQ, 0);
            rel(*this, matrix.col(1), IRT_EQ, 0);
            rel(*this, matrix.row(n-1), IRT_EQ, 0);
            rel(*this, matrix.col(n-1), IRT_EQ, 0);
            rel(*this, matrix.row(n-2), IRT_EQ, 0);
            rel(*this, matrix.col(n-2), IRT_EQ, 0);

            int k = 0;
            // Game Of Life constraints
            for (int i = 1; i < n-1; ++i) {
                for (int j = 1; j < n-1; ++j) {
                    IntVar c(*this, 0, n);
                    BoolVarArgs neighbors;
                    neighbors << matrix(i-1,j-1) << matrix(i-1,j)
                            << matrix(i-1,j+1) << matrix(i,j-1)
                            << matrix(i,j+1) << matrix(i+1,j-1)
                            << matrix(i+1,j) << matrix(i+1,j+1);
                    // Count neighbors to this cell
                    rel(*this, sum(neighbors) == c);
                    // Make sure this cell is live when it is supposed to be
                    rel(*this, matrix(i,j) >> (c == 2 || c == 3));
                    rel(*this, !matrix(i,j) >> (c != 3));
                    
                    // Every 3x3 square, calculate sum of that minisquare
                    if (i % 3 == 2 && j % 3 == 2 && i < n-2 && j < n-2) {
                        rel(*this, minicells[k] == (matrix(i,j) + matrix(i,j+1) + matrix(i,j+2) +
                                matrix(i+1,j) + matrix(i+1,j+1) + matrix(i+1,j+2) +
                                matrix(i+2,j) + matrix(i+2,j+1) + matrix(i+2,j+2)));
                        k++;
                    }
                }
            }

            // Branch
            branch(*this, cells, INT_VAR_NONE, INT_VAL_MAX);
        }

        GameOfLife(bool share, GameOfLife& sp) : Script(share, sp) {
            cells.update(*this, share, sp.cells);
            minicells.update(*this, share, sp.minicells);
        }

        virtual void constrain(const Space& b) {
            const GameOfLife& gol = static_cast<const GameOfLife&>(b);
            rel(*this, sum(cells) > sum(gol.cells));
            rel(*this, sum(minicells) > sum(gol.minicells));
        }

        virtual Space* copy(bool share) {
            return new GameOfLife(share, *this);
        }

        virtual void print(std::ostream& os) const {
            os << "Game of Life cells" << std::endl;
            int n = sqrt(cells.size());
            int alive = 0;
            Matrix<BoolVarArgs> matrix(cells, n, n);
            for (int i = 2; i < n - 2; ++i) {
                for (int j = 2; j < n - 2; ++j) {
                    if (matrix(i, j).assigned()) {
                        if (matrix(i, j).val() == 1) {
                            os << "X";
                            alive++;
                        } else {
                            os << ".";
                        }
                    }
                }
                os << std::endl;
            }
            os << "Alive " << alive << std::endl;
        }
};

int main(int argc, char* argv[]) {
    SizeOptions opt("GameOfLife");
    opt.solutions(0);
    opt.size(5);
    opt.parse(argc,argv);
    Script::run<GameOfLife, BAB, SizeOptions>(opt);
    return 0;
}
