import std;
using namespace std;

class Matrix{
private:
    int _rows;
    int _cols;
    vector<vector<double>> _data;

public:
    Matrix(int r, int c){
        if (r <= 0 || c <= 0)
        {
            throw invalid_argument("Неверный размер матрицы");
        }

        _rows = r;
        _cols = c;

        _data.assign(r, vector<double>(c, 0));
    }
	Matrix() : _rows(0), _cols(0) {}

    int rows() const{
        return _rows;
    }

    int cols() const{
        return _cols;
    }

    double& operator()(int r, int c){
        if (r < 0 || r >= _rows ||
            c < 0 || c >= _cols)
        {
            throw out_of_range("Неверный индекс");
        }

        return _data[r][c];
    }

    const double& operator()(int r, int c) const{
        if (r < 0 || r >= _rows ||
            c < 0 || c >= _cols)
        {
            throw out_of_range("Неверный индекс");
        }

        return _data[r][c];
    }
	Matrix operator*(const Matrix& other) const{
		if(_cols != other._rows){
			throw invalid_argument("нельзя перемножить матрицы");
		}
		Matrix res(_rows, other._cols);
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < other._cols; j++){
				for(int k = 0; k < _cols; k++){
					res(i, j) += (*this)(i, k) * other(k, j);
				}
			}
		}
		return res;
	}
	void LoadFromFile(const string& filename){
		ifstream file(filename);
		if(!file.is_open()){
			throw runtime_error("не удалось открыть файл");
		}
		file >> _rows >> _cols;
		_data.assign(_rows, vector<double>(_cols, 0));
		for(int i =0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				file >> _data[i][j];
			}
		}
	}
	void SaveToFile(const string& filename) const{
		ofstream file(filename);
		if(!file.is_open()){
			throw runtime_error("не удалось открыть файл");
		}
		file << _rows << " " << _cols << "\n";
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				file << _data[i][j] << " ";
			}
			file << "\n";
		}
	}

    friend ostream& operator<<(ostream& out, const Matrix& matrix){
        for (int i = 0; i < matrix._rows; i++)
        {
            for (int j = 0; j < matrix._cols; j++)
            {
                out << matrix._data[i][j] << " ";
            }

            out << "\n";
        }

        return out;
    }
};



int main(){
	try{
    cout << "Текущая папка: " << filesystem::current_path() << "\n";

    Matrix A;
    Matrix B;

    A.LoadFromFile("matrixA.txt");
    B.LoadFromFile("matrixB.txt");

    Matrix C = A * B;

    cout << "A:\n";
    cout << A;

    cout << "\nB:\n";
    cout << B;

    cout << "\nC:\n";
    cout << C;
	C.SaveToFile("result.txt");
	}catch(const exception& e){
		cerr << "ошибка" << e.what() << endl;
	}

    return 0;
}