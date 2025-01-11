#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // -------- Bước 1: Nhập dữ liệu --------
    int n;
    double Dmax;
    cin >> n >> Dmax;

    vector<double> x(n), w(n);
    for(int i = 0; i < n; i++) {
        cin >> x[i] >> w[i];
    }

    // -------- Bước 2: Tạo lưới rời rạc (grid) --------
    double minX = *min_element(x.begin(), x.end()) - 10.0;
    double maxX = *max_element(x.begin(), x.end()) + 10.0;
    double step = 1.0;  // có thể chỉnh step nhỏ hơn để tăng độ chính xác

    // Sinh các giá trị trong [minX, maxX] với bước step
    vector<double> grid;
    for (double val = minX; val <= maxX; val += step) {
        grid.push_back(val);
    }
    int M = (int)grid.size();

    // -------- Bước 3: Chuẩn bị mảng DP --------
    // dp_prev[j] = chi phí tối ưu khi lá (i-1) chọn grid[j]
    // dp_curr[j] = chi phí tối ưu khi lá i chọn grid[j]
    // parent[i][j] = vị trí k giúp dp[i][j] đạt giá trị tối ưu
    const double INF = 1e18;
    vector<double> dp_prev(M, INF), dp_curr(M, INF);
    // Để truy vết: parent[i][j] => index k
    vector<vector<int>> parent(n, vector<int>(M, -1));

    // -------- Bước 4: Khởi tạo DP cho lá đầu tiên (i = 0) --------
    for (int j = 0; j < M; j++) {
        dp_prev[j] = w[0] * fabs(x[0] - grid[j]);
        // i=0, không có lá trước, nên parent[0][j] = -1 (không cần thiết lắm)
        parent[0][j] = -1;
    }

    // -------- Bước 5: Tính DP cho i = 1..n-1 --------
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < M; j++) {
            double yj = grid[j];
            double leftVal  = yj - Dmax;
            double rightVal = yj + Dmax;

            // Tìm chỉ số [k_left..k_right] thoả |grid[k] - yj| <= Dmax
            int k_left = int(lower_bound(grid.begin(), grid.end(), leftVal) - grid.begin());
            int k_right = int(upper_bound(grid.begin(), grid.end(), rightVal) - grid.begin()) - 1;
            if (k_left < 0)   k_left = 0;
            if (k_right >= M) k_right = M - 1;

            // Tìm min dp_prev[k] trong đoạn [k_left..k_right]
            double best_prev = INF;
            int best_k = -1;
            for (int k = k_left; k <= k_right; k++) {
                if (dp_prev[k] < best_prev) {
                    best_prev = dp_prev[k];
                    best_k = k;
                }
                else if (fabs(dp_prev[k] - best_prev) < 1e-12) {
                    if (k < best_k) {
                        best_k = k;
                    }
                }

            }
            // Tính chi phí khi lá i chọn grid[j]
            dp_curr[j] = best_prev + w[i] * fabs(x[i] - yj);
            // Ghi lại parent
            parent[i][j] = best_k;
        }
        // Chuyển dp_curr -> dp_prev và reset dp_curr
        dp_prev = dp_curr;
        fill(dp_curr.begin(), dp_curr.end(), INF);
    }

    // -------- Bước 6: Lấy kết quả nhỏ nhất và truy vết --------
    // 1) Tìm chỉ số j_min để dp_prev[j_min] nhỏ nhất
    double ans = INF;
    int best_j = -1;
    for (int j = 0; j < M; j++) {
        if (dp_prev[j] < ans) {
            ans = dp_prev[j];
            best_j = j;
        }
    }

    // 2) Truy vết để tìm y_i
    // Tạo mảng y (kích thước n), ta sẽ lần ngược từ i = n-1 -> 0
    vector<double> y(n);
    int curr_j = best_j;
    for (int i = n - 1; i >= 0; i--) {
        y[i] = grid[curr_j];         // vì lá i chọn grid[curr_j]
        curr_j = parent[i][curr_j];  // sang vị trí k của lá (i-1)
    }

    // -------- Bước 7: Xuất kết quả --------
    cout << fixed << setprecision(6);
    cout << "Min cost = " << ans << "\n";
    cout << "y sequence:\n";
    for (int i = 0; i < n; i++) {
        cout << "y[" << i << "] = " << y[i] << "\n";
    }

    return 0;
}
