import std; 
using namespace std;

import std;
using namespace std;

class Matrix{
private:
	int _rows;
	int _cols;
	vector<vector<double>> _data;
public:
	Matrix(int r, int c){
		if(r < 0 || c < 0){
			throw invalid_argument("не верный размер");
		}
		_rows = r;
		_cols = c;
		_data.assign(r, vector<double>(c, 0));
	}
	
	int rows() const {
		return _rows;
	}
	int cols() const {
		return _cols;
	}
	
    double& operator()(int r, int c) {
        if (r < 0 || r >= _rows || c < 0 || c >= _cols) {
            throw out_of_range("неверный индекс");
        }
        return _data[r][c];
	}
	double Trace() const {
		if(_rows != _cols){
			throw invalid_argument("матрица не квадратная");
		}
		double sum = 0;
		for(int i = 0; i < _rows; i++){
			sum += _data[i][i];
		}
		return sum;
	}
	

	
	Matrix Transpose(){
		Matrix newMatrix(_cols, _rows);
		
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				newMatrix(j, i) = _data[j][i];
			}
		}
		return newMatrix;

	}
	
	const double& operator()(int r, int c) const {
		if(r < 0 || c < 0){
			throw invalid_argument("не верный индекс");
		}
		return (_data[r])[c];
	}
	
	Matrix operator+(const Matrix& other) {
		if(_rows != other._rows || _cols != other._cols){
			throw invalid_argument("у матриц разный размер");
		}
		Matrix res(_rows, _cols);
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j ++){
				res(i, j) = _data[i][j] + other._data[i][j];
			}
		}
		return res;
	}
	
	Matrix& operator+=(const Matrix& other){
		if(_rows != other._rows || _cols != other._cols){
			throw invalid_argument("у матриц разный размер");
		}
		
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j ++){
				_data[i][j] += other._data[i][j];
			}
		}
		return *this;
		
		
	}
	
	friend ostream& operator<<(ostream& out, const Matrix& other){
		for(int i = 0; i < other._rows; i++){
			for(int j = 0; j < other._cols; j++){
				out << other._data[i][j] << " ";
			}
			out << "\n";
		}
		return out;
	}
	
};

int main() {
    try {
        Matrix m(3, 3);

        m(0, 0) = 1;
        m(5, 5) = 10; // здесь выбросится исключение
    }
    catch (const out_of_range& e) {
        cout << e.what() << endl;
    }


    return 0;
}