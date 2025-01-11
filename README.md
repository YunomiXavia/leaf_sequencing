# Tài liệu mô tả chi tiết giải pháp cho bài toán "Leaf Move Problem"

## 1. Mô tả bài toán

Cho $n$ lá, mỗi lá có một vị trí thực $\,x_i$ và trọng số $\,w_i$. Ta muốn “di chuyển” (hay **gán**) lá thứ $i$ vào vị trí $\,y_i$ sao cho:

1. $\bigl|\,y_{i+1} - y_i\,\bigr| \;\le\; D_{\text{max}}$, với mọi $i = 1, 2, \ldots, n-1$.
2. Tối ưu (minimize) tổng sai số có trọng số:
   $
   \sum_{i=1}^{n} w_i \cdot \bigl|\,x_i - y_i\,\bigr|.
   $

Nói cách khác, ta cần tìm $y_1, y_2, \ldots, y_n$ thỏa mãn ràng buộc khoảng cách liên tiếp và tối thiểu hóa tổng sai số $\bigl|\,x_i - y_i\,\bigr|$ (có nhân trọng số $w_i$).

### Ví dụ minh họa

- Giả sử $D_{\text{max}} = 3$.
- Danh sách lá:
    - Lá 1: $x_1 = 44,\; w_1 = 1$
    - Lá 2: $x_2 = 4,\;\;\, w_2 = 1$
    - Lá 3: $x_3 = 10,\; w_3 = 1000$
    - Lá 4: $x_4 = 4,\;\;\, w_4 = 1$
- Một lời giải có thể là $ (y_1, y_2, y_3, y_4) = (16, 13, 10, 7) $.
    - Kiểm tra ràng buộc: $\lvert\,y_2 - y_1\rvert = 3 \le 3$, $\lvert\,y_3 - y_2\rvert = 3 \le 3$, $\lvert\,y_4 - y_3\rvert = 3 \le 3$.
    - Tổng chi phí $\;= w_1\lvert 44 - 16\rvert + w_2\lvert 4 - 13\rvert + w_3\lvert 10 - 10\rvert + w_4\lvert 4 - 7\rvert$.

Vì trong bài toán này, có thể tồn tại **nhiều nghiệm tối ưu khác nhau** (cho chi phí tối ưu như nhau), nên **lời giải** thu được có thể **không duy nhất**.

---

## 2. Ý tưởng tiếp cận bài toán

### 2.1. Rời rạc hóa miền giá trị

Vì các $y_i$ có thể nhận giá trị **thực** bất kỳ, nên để triển khai Quy hoạch động (DP) ta thường:

1. **Giới hạn** miền tìm kiếm vào một khoảng $[\min x - \delta, \max x + \delta]$.
2. **Chia** khoảng này thành nhiều điểm rời rạc với một bước nhảy $\text{step}$.
3. Tạo mảng `grid` gồm các giá trị rời rạc này, và **bắt buộc** $y_i$ phải chọn trong `grid`.

### 2.2. Định nghĩa DP

- Ký hiệu $\text{grid}[j]$ là giá trị thứ $j$ trong lưới.
- Đặt $\text{dp}[i][j]$ = **chi phí tối ưu** khi lá thứ $i$ được đặt ở $\text{grid}[j]$.

Công thức chuyển trạng thái:

$
\text{dp}[i][j]
\;=\;
\min_{\substack{k:\\|\text{grid}[j] - \text{grid}[k]| \le D_{\max}}}
\Bigl(\text{dp}[i-1][k]\Bigr)
\;+\;
w_i \cdot \bigl|\,x_i - \text{grid}[j]\,\bigr|.
$

- Nghĩa là, để đặt lá $i$ ở $\text{grid}[j]$, lá $i-1$ phải ở một vị trí $\text{grid}[k]$ gần $\text{grid}[j]$ trong phạm vi $D_{\max}$.
- Sau khi tìm $\text{dp}[i][j]$, ta chọn giá trị nhỏ nhất trong số các $k$ thỏa mãn.

### 2.3. Lưu vết (backtrack) để tìm dãy $\{y_i\}$

- Để **khôi phục** được nghiệm $y_i$, ta dùng một mảng `parent[i][j]`, ghi lại **vị trí $k$** (trong `grid`) giúp $\text{dp}[i][j]$ đạt giá trị min.
- Sau khi tính xong, ta dò lại $\text{dp}[n-1][j]$ để tìm chi phí nhỏ nhất (gọi là `best_j`).
- Từ `best_j`, lần ngược về `i = n-1, n-2, ..., 0` (dùng `parent[i][best_j]`) để tìm ra dãy $y_i$.

---

## 3. Vấn đề “tie-break” (chọn $k$ nếu chi phí bằng nhau)

Trong quá trình tìm $\min$, có thể xảy ra tình huống **nhiều** chỉ số $k$ cùng cho một giá trị $\text{dp}[i-1][k]$ (chi phí) **bằng nhau**. Lúc này, chúng ta cần quy ước chọn $k$ như thế nào.

Ví dụ, trong đoạn code:

```cpp
double best_prev = INF;
int best_k = -1;
for (int k = k_left; k <= k_right; k++) {
    if (dp_prev[k] < best_prev) {
        best_prev = dp_prev[k];
        best_k = k;
    }
}
```
- Ở đây, nếu `dp_prev[k] == best_prev` thì ta **giữ nguyên** `best_k` (không cập nhật).
- Kết quả: **k đầu tiên** (trong khoảng $[k_{\text{left}}, k_{\text{right}}]$) mà ta gặp có chi phí $`dp_prev[k] = best_prev`$ sẽ được chọn.

Tuy nhiên, nếu **muốn** khi tie thì **chọn $k$ lớn hơn** (hoặc **nhỏ hơn**) để “bẻ” lời giải, ta có thể **thay đổi** như sau:

```cpp
for (int k = k_left; k <= k_right; k++) {
    if (dp_prev[k] < best_prev) {
        // Tìm được chi phí nhỏ hơn => cập nhật
        best_prev = dp_prev[k];
        best_k = k;
    } 
    else if (fabs(dp_prev[k] - best_prev) < 1e-12) {
        // Chi phí ngang nhau (tie)
        // Thay vì giữ nguyên, ta sẽ có tiêu chí phụ:
        // 1) Chọn k lớn hơn
        if (k > best_k) {
            best_k = k;
        }
        // 2) Hoặc chọn k nhỏ hơn (tùy ý)
        // if (k < best_k) {
        //     best_k = k;
        // }
    }
}
```

Việc **lựa chọn tie-break** này sẽ ảnh hưởng đến **dãy $\{y_i\}$ cuối cùng** (mặc dù chi phí tối ưu không đổi).

> - **Tie-break “chọn k đầu tiên”**: Kết quả có xu hướng ổn định, vì chọn “vừa” gặp cost min là dừng.
> - **Tie-break “chọn k lớn nhất”**: Khi có nhiều vị trí cùng cost, nghiệm “ngả” về **grid** có chỉ số (hoặc giá trị) cao hơn.
> - **Tie-break “chọn k nhỏ nhất”**: Ngược lại, nghiệm “ngả” về **grid** có chỉ số (hoặc giá trị) thấp hơn.

Ta cũng có thể **đổi chiều duyệt** (từ `M-1` đến `0`) thay vì `0` đến `M-1` để đạt hiệu ứng tương tự.

### Tại sao cần quan tâm “tie-break”?

Vì bài toán **có thể có nhiều** nghiệm tối ưu khác nhau (do các lá có thể dịch chuyển trong giới hạn $D_{\text{max}}$), đôi khi ta muốn “ưu tiên” dãy $\{y_i\}$ theo một tiêu chí phụ (chẳng hạn, chọn $y_1$ càng gần $x_1$ càng tốt, hoặc chọn $\{y_i\}$ càng lớn càng tốt...). Khi đó, cơ chế tie-break sẽ giúp **định hình** nghiệm cuối cùng.

---

## 4. Code C++ chi tiết (có lưu vết & tie-break minh họa)

Dưới đây là ví dụ code C++ hoàn chỉnh. Trong đó, phần **tie-break** được chú thích rõ ràng, ta có thể điều chỉnh để thay đổi kết quả lời giải.

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
    // parent[i][j] = chỉ số k giúp dp[i][j] đạt giá trị tối ưu
    const double INF = 1e18;
    vector<double> dp_prev(M, INF), dp_curr(M, INF);
    vector<vector<int>> parent(n, vector<int>(M, -1));

    // ===== Bước 4: Khởi tạo cho lá đầu tiên (i = 0) =====
    for (int j = 0; j < M; j++) {
        dp_prev[j] = w[0] * fabs(x[0] - grid[j]);
        parent[0][j] = -1; 
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

            // Tìm min trong dp_prev[k] (k_left <= k <= k_right)
            double best_prev = INF;
            int best_k = -1;
            for (int k = k_left; k <= k_right; k++) {
                // So sánh dp_prev[k] với best_prev
                if (dp_prev[k] < best_prev) {
                    best_prev = dp_prev[k];
                    best_k = k;
                } 
                else if (fabs(dp_prev[k] - best_prev) < 1e-12) {
                    // ======== TIE-BREAK VÍ DỤ ========
                    // Nếu cùng chi phí, chọn k lớn hơn
                    // (có thể chuyển thành k < best_k hay k random,... tùy ý)
                    if (k > best_k) {
                        best_k = k;
                    }
                }
            }

            // Lúc này, best_prev là dp_prev[k] tốt nhất
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
    for (int j = 0; j < M; j++) {
        if (dp_prev[j] < ans) {
            ans = dp_prev[j];
            best_j = j;
        }
    }

    // Truy vết ngược để khôi phục dãy y
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
```

Trong đoạn `// TIE-BREAK VÍ DỤ`, có thể:

- **Chọn k lớn hơn** khi tie:
  ```cpp
  if (k > best_k) best_k = k;
  ```
- **Chọn k nhỏ hơn** khi tie:
  ```cpp
  if (k < best_k) best_k = k;
  ```
- Hoặc thậm chí chọn **ngẫu nhiên** (random) một trong các k tie.

Việc thay đổi này **không** làm tăng chi phí tối ưu, mà chỉ thay đổi **phân bố $\{y_i\}$**. Qua đó, có thể điều khiển “nghiệm” cuối cùng theo nhu cầu riêng.