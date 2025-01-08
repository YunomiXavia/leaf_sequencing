# Tài liệu mô tả chi tiết giải pháp cho bài toán "Leaf Move Problem"

## 1. Mô tả bài toán

Cho $n$ lá, mỗi lá có một vị trí thực $x_i$ và trọng số $w_i$. Ta muốn di chuyển (hay "gán") lá thứ $i$ vào vị trí $y_i$ sao cho:

1. $\bigl|\,y_{i+1} - y_i\,\bigr| \le D_{\text{max}}$, với mọi $i = 1, 2, \ldots, n-1$.
2. Tối ưu (minimize) tổng sai số có trọng số:
   $$
   \sum_{i=1}^{n} w_i \cdot \bigl|\,x_i - y_i\,\bigr|.
   $$

Nói cách khác, ta cần tìm $y_1, y_2, \ldots, y_n$ thỏa mãn ràng buộc khoảng cách liên tiếp và tối thiểu hóa tổng sai số $\bigl|\,x_i - y_i\,\bigr|$ (có nhân trọng số $w_i$).

### Ví dụ minh họa

- Giả sử $D_{\text{max}} = 3$.
- Danh sách lá:
    - Lá 1: $x_1 = 44,\; w_1 = 1$
    - Lá 2: $x_2 = 4,\;\;\, w_2 = 1$
    - Lá 3: $x_3 = 10,\; w_3 = 1000$
    - Lá 4: $x_4 = 4,\;\;\, w_4 = 1$
- Một lời giải có thể là:
    - $y_1 = 16$
    - $y_2 = 13$
    - $y_3 = 10$
    - $y_4 = 7$
- Ta thấy $\bigl|y_2 - y_1\bigr| = 3 \le D_{\text{max}}$, $\bigl|y_3 - y_2\bigr| = 3 \le D_{\text{max}}$, $\bigl|y_4 - y_3\bigr| = 3 \le D_{\text{max}}$.

---

## 2. Ý tưởng tiếp cận bài toán

### 2.1. Rời rạc hóa miền giá trị

- Vì các $y_i$ có thể nhận giá trị thực bất kỳ, nên để triển khai giải quyết bằng Quy hoạch động (DP), ta rời rạc hóa chúng thành một tập hữu hạn.
- Cách làm:
    - Tìm giá trị nhỏ nhất (minX) và lớn nhất (maxX) trong dãy $x_i$,
    - Mở rộng biên một chút (ví dụ: $\pm 10$) cho an toàn,
    - Chia khoảng này thành các điểm cách nhau một bước step (ví dụ: 1.0).
- Tập giá trị này được gọi là `grid`. Sau đó, mỗi $y_i$ buộc phải là một phần tử trong `grid`.

### 2.2. Công thức Quy hoạch động (Dynamic Programming)

- Định nghĩa `dp[i][j]`: chi phí tối ưu nếu lá thứ $i$ (chỉ số 0-based) được đặt tại vị trí `grid[j]`.
- Khi chuyển từ lá $i-1$ sang lá $i$, ta cần ràng buộc $\bigl|\text{grid}[j] - \text{grid}[k]\bigr| \le D_{\text{max}}$.
- Công thức:
  $$
  dp[i][j]
  \;=\;
  \min_{k:|\text{grid}[j]-\text{grid}[k]|\le D_{\max}}
  \Bigl( dp[i-1][k] \Bigr)
  \;+\;
  w_i \cdot \bigl|\,x_i - \text{grid}[j]\,\bigr|.
  $$

### 2.3. Lưu vết để khôi phục nghiệm $y_i$

- Để tìm chính xác $y_i$ cho từng lá, ta cần một mảng `parent[i][j]`:
    - `parent[i][j]` = chỉ số $k$ (trong `grid`) sao cho $dp[i][j]$ đạt giá trị tối ưu (tức là lá $i$ ở vị trí `grid[j]` thì lá $i-1$ tốt nhất nằm ở `grid[k]`).
- Sau khi chạy xong DP, ta tìm giá trị nhỏ nhất ở hàng cuối (hoặc mảng `dp_prev` của lá $n-1$). Gọi đó là `best_j`.
- Ta lần ngược $i$ từ $n-1$ về $0$, sử dụng `best_j = parent[i][best_j]` để ra chỉ số cho lá trước.
- Quá trình này giúp ta in ra được dãy $y_i$.

### 2.4. Mã giả (Pseudo-code) tóm tắt

1. **Tạo lưới** `grid` trong khoảng $[\min x - \delta, \max x + \delta]$ với bước `step`.
2. **Khởi tạo** `dp_prev[j] = w[0] * |x[0] - grid[j]|`, `parent[0][j] = -1`.
3. **Vòng lặp i = 1..n-1**
    - Với mỗi $j$, tìm min trong `dp_prev[k]` với điều kiện $|\text{grid}[j] - \text{grid}[k]| \le D_{\text{max}}$,
    - Cập nhật `dp_curr[j]` và ghi `parent[i][j]`.
    - Sao chép `dp_curr -> dp_prev`.
4. **Tìm đáp án** bằng cách lấy min của `dp_prev[j]`.
5. **Truy vết** `parent` để khôi phục dãy $y_i$.

---

## 3. Code C++ chi tiết có lưu vết khôi phục $y_i$

Dưới đây là ví dụ code C++ hoàn chỉnh:

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // ===== Bước 1: Nhập dữ liệu =====
    int n;
    double Dmax;
    cin >> n >> Dmax;

    vector<double> x(n), w(n);
    for(int i = 0; i < n; i++) {
        cin >> x[i] >> w[i];
    }

    // ===== Bước 2: Tạo lưới rời rạc (grid) =====
    double minX = *min_element(x.begin(), x.end()) - 10.0;
    double maxX = *max_element(x.begin(), x.end()) + 10.0;
    double step = 1.0;  // có thể chỉnh nhỏ hơn để tăng độ chính xác

    vector<double> grid;
    for (double val = minX; val <= maxX; val += step) {
        grid.push_back(val);
    }
    int M = (int)grid.size();

    // ===== Bước 3: Chuẩn bị mảng DP và parent =====
    // dp_prev[j] = chi phí tối ưu cho lá (i-1), chọn grid[j]
    // dp_curr[j] = chi phí tối ưu cho lá i, chọn grid[j]
    // parent[i][j] = chỉ số k tốt nhất giúp dp[i][j] đạt giá trị tối ưu
    const double INF = 1e18;
    vector<double> dp_prev(M, INF), dp_curr(M, INF);
    vector<vector<int>> parent(n, vector<int>(M, -1));

    // ===== Bước 4: Khởi tạo cho lá đầu tiên (i = 0) =====
    for (int j = 0; j < M; j++) {
        dp_prev[j] = w[0] * fabs(x[0] - grid[j]);
        parent[0][j] = -1; // lá đầu tiên không có cha
    }

    // ===== Bước 5: Tính DP cho i = 1..n-1 =====
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < M; j++) {
            double yj = grid[j];
            // Xác định khoảng [k_left..k_right] sao cho |grid[k] - yj| <= Dmax
            double leftVal  = yj - Dmax;
            double rightVal = yj + Dmax;

            int k_left = int(lower_bound(grid.begin(), grid.end(), leftVal) - grid.begin());
            int k_right = int(upper_bound(grid.begin(), grid.end(), rightVal) - grid.begin()) - 1;
            if (k_left < 0)   k_left = 0;
            if (k_right >= M) k_right = M - 1;

            // Duyệt k trong [k_left..k_right], lấy dp_prev[k] bé nhất
            double best_prev = INF;
            int best_k = -1;
            for (int k = k_left; k <= k_right; k++) {
                if (dp_prev[k] < best_prev) {
                    best_prev = dp_prev[k];
                    best_k = k;
                }
            }

            // Gán dp_curr[j] và parent
            dp_curr[j] = best_prev + w[i] * fabs(x[i] - yj);
            parent[i][j] = best_k;
        }
        // Copy dp_curr -> dp_prev
        dp_prev = dp_curr;
        // Reset dp_curr cho lượt tiếp theo
        fill(dp_curr.begin(), dp_curr.end(), INF);
    }

    // ===== Bước 6: Tìm kết quả tối ưu và truy vết =====
    double ans = INF;
    int best_j = -1;
    // Tìm j nào cho dp_prev[j] nhỏ nhất
    for (int j = 0; j < M; j++) {
        if (dp_prev[j] < ans) {
            ans = dp_prev[j];
            best_j = j;
        }
    }

    // Truy vết từ lá cuối (n-1) về lá 0, khôi phục y[i]
    vector<double> y(n);
    int curr_j = best_j;
    for (int i = n - 1; i >= 0; i--) {
        y[i] = grid[curr_j];
        curr_j = parent[i][curr_j];
    }

    // ===== Bước 7: In kết quả =====
    cout << fixed << setprecision(6);
    cout << "Min cost = " << ans << "\n";
    cout << "Sequence y:\n";
    for (int i = 0; i < n; i++) {
        cout << "y[" << i << "] = " << y[i] << "\n";
    }

    return 0;
}
