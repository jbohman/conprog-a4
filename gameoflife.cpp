#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class GameOfLife : public Script {
    protected:
        BoolVarArray cells;

    public:
        GameOfLife(const SizeOptions& opt) : cells(*this, opt.size() + 4 * opt.size() + 4, 0, 1) {
            // Constraints

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
            Matrix<BoolVarArgs> matrix(cells, opt.size() + 4, opt.size() + 4);
            for (int i = 2; i < opt.size() + 2; ++i) {
                for (int j = 2; j < opt.size() + 2; ++j) {
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
