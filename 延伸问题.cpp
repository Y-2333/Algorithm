#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>
using namespace std;

int lengthOfLIS_DP(vector<int>& nums, int k, vector<int>& lis) {
    int n = nums.size();
    vector<int> dp(n, 1), prev(n, -1);
    int maxLength = 1, endIndex = 0;

    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[i] > nums[j] && nums[i] - nums[j] <= k) {
                if (dp[i] < dp[j] + 1) {
                    dp[i] = dp[j] + 1;
                    prev[i] = j;
                }
            }
        }
        if (maxLength < dp[i]) {
            maxLength = dp[i];
            endIndex = i;
        }
    }

    for (int i = endIndex; i != -1; i = prev[i]) {
        lis.push_back(nums[i]);
    }
    reverse(lis.begin(), lis.end());

    return maxLength;
}

class FenwickTree {
private:
    int n;
    vector<int> a;
    vector<int> c;

public:
    FenwickTree(int size) : n(size), a(size + 1), c(size + 1) {}

    int lowbit(int i) {
        return i & (-i);
    }

    void add(int x, int y) {
        a[x] = max(a[x], y);
        update(x);
    }

    void update(int x) {
        while (x <= n) {
            c[x] = a[x];
            int lx = lowbit(x);
            for (int i = 1; i < lx; i <<= 1) {
                c[x] = max(c[x], c[x - i]);
            }
            x += lowbit(x);
        }
    }

    int query(int x, int y) {
        int lis = 0;
        while (y >= x) {
            lis = max(a[y], lis);
            y--;
            for (; y - lowbit(y) >= x; y -= lowbit(y)) {
                lis = max(lis, c[y]);
            }
        }
        return lis;
    }
};

int lengthOfLIS_BIT(vector<int>& nums, int k, vector<int>& lisSequence) {
    if (nums.empty()) return 0;

    int n = nums.size();
    int maxNum = *max_element(nums.begin(), nums.end());
    FenwickTree fenwickTree(maxNum + 1);
    vector<int> dp(nums.size());
    vector<int> previousIndex(nums.size(), -1);

    int lis = 0;
    int lisEnd = -1;

    for (int i = 0; i < nums.size(); ++i) {
        int num = nums[i];
        int queryResult = fenwickTree.query(max(num - k + 1, 1), num);
        int newLength = queryResult + 1;
        dp[i] = newLength;

        if (newLength > lis) {
            lis = newLength;
            lisEnd = i;
        }

        fenwickTree.add(num + 1, newLength);

        for (int j = i - 1; j >= 0; --j) {
            if (nums[j] <= nums[i] && nums[i] - nums[j] <= k && dp[j] == queryResult) {
                previousIndex[i] = j;
                break;
            }
        }
    }

    lisSequence.clear();
    for (int i = lisEnd; i != -1; i = previousIndex[i]) {
        lisSequence.push_back(nums[i]);
    }
    reverse(lisSequence.begin(), lisSequence.end());

    return lis;
}

class SegmentTree {
    vector<int> tree;
    int size;

public:
    SegmentTree(int n) {
        size = n;
        tree.resize(4 * n, 0);
    }

    void update(int index, int value, int node, int nodeLeft, int nodeRight) {
        if (nodeLeft == nodeRight) {
            tree[node] = value;
        }
        else {
            int mid = (nodeLeft + nodeRight) / 2;
            if (index <= mid) {
                update(index, value, 2 * node, nodeLeft, mid);
            }
            else {
                update(index, value, 2 * node + 1, mid + 1, nodeRight);
            }
            tree[node] = max(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void update(int index, int value) {
        update(index, value, 1, 1, size);
    }

    int query(int queryLeft, int queryRight, int node, int nodeLeft, int nodeRight) {
        if (queryRight < nodeLeft || queryLeft > nodeRight) {
            return 0; 
        }
        if (queryLeft <= nodeLeft && nodeRight <= queryRight) {
            return tree[node];
        }
        int mid = (nodeLeft + nodeRight) / 2;
        return max(query(queryLeft, queryRight, 2 * node, nodeLeft, mid),
            query(queryLeft, queryRight, 2 * node + 1, mid + 1, nodeRight));
    }

    int query(int queryLeft, int queryRight) {
        return query(queryLeft, queryRight, 1, 1, size);
    }
};

int lengthOfLISWith_Segment_Tree(vector<int>& nums, int k, vector<int>& lisSequence) {
    if (nums.empty()) return 0;

    int maxVal = *max_element(nums.begin(), nums.end());
    SegmentTree segmentTree(maxVal);

    vector<int> dp(nums.size(), 0); 
    vector<int> prevIndex(nums.size(), -1); 

    int lis = 0;
    int lisEndIndex = -1;

    for (int i = 0; i < nums.size(); ++i) {
        int num = nums[i];
        int maxLength = 0;
        int maxIndex = -1;
        if (num > 1) {
            int left = max(1, num - k);
            int right = num - 1;

            for (int j = 0; j < i; ++j) { 
                if (nums[j] >= left && nums[j] <= right && dp[j] > maxLength) {
                    maxLength = dp[j];
                    maxIndex = j;
                }
            }
        }
        dp[i] = maxLength + 1;
        prevIndex[i] = maxIndex;
        segmentTree.update(num, dp[i]);
        if (dp[i] > lis) {
            lis = dp[i];
            lisEndIndex = i;
        }
    }
    int currentIndex = lisEndIndex;
    while (currentIndex != -1) {
        lisSequence.push_back(nums[currentIndex]);
        currentIndex = prevIndex[currentIndex];
    }
    reverse(lisSequence.begin(), lisSequence.end());

    return lis;
}

int main() {
    int n;
    cout << "Length of Sequence: ";
    cin >> n;
    int m;
    vector<int> nums;
    cout << "Input numbers: ";
    for (int i = 0; i < n; i++) {
        cin >> m;
        nums.push_back(m);
    }
    int k;
    cout << "k: ";
    cin >> k;

    vector<int> lis;
    cout << "Length of LIS (DP O(n^2)): " << lengthOfLIS_DP(nums, k,lis) << endl;
    cout << "LIS: ";
    for (int num : lis) cout << num << " ";
    cout << endl;

    lis.clear();
    cout << "Length of LIS (BIT O(n (log n)^2)): " << lengthOfLIS_BIT(nums, k, lis) << endl;
    cout << "LIS: ";
    for (int num : lis) cout << num << " ";
    cout << endl;

    lis.clear();
    cout << "Length of LIS (Segment_Tree O(n log n)): " << lengthOfLISWith_Segment_Tree(nums, k,lis) << endl;
    cout << "LIS: ";
    for (int num : lis) cout << num << " ";
    cout << endl;

    return 0;
}
