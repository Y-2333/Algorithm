#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;

vector<int> reconstruct_LIS(const vector<int>& nums, const vector<int>& prev, int end_index) {
    vector<int> lis;
    for (int i = end_index; i >= 0; i = prev[i]) {
        lis.push_back(nums[i]);
        if (prev[i] == i) break;
    }
    reverse(lis.begin(), lis.end());
    return lis;
}

int lengthOfLIS_DP(vector<int>& nums, vector<int>& lis) {
    if (nums.empty()) return 0;
    int n = nums.size();
    vector<int> dp(n, 1);
    vector<int> prev(n, -1);
    int max_len = 1, max_index = 0;
    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[i] > nums[j]) {
                if (dp[i] < dp[j] + 1) {
                    dp[i] = dp[j] + 1;
                    prev[i] = j;
                }
            }
        }
        if (dp[i] > max_len) {
            max_len = dp[i];
            max_index = i;
        }
    }
    lis = reconstruct_LIS(nums, prev, max_index);
    return max_len;
}

int lengthOfLIS_Greedy(vector<int>& nums, vector<int>& lis) {
    vector<int> dp;
    vector<int> dp_indices;
    vector<int> prev(nums.size(), -1);
    for (int i = 0; i < nums.size(); ++i) {
        auto it = lower_bound(dp.begin(), dp.end(), nums[i]);
        int idx = distance(dp.begin(), it);
        if (it == dp.end()) {
            dp.push_back(nums[i]);
            dp_indices.push_back(i);
        }
        else {
            *it = nums[i];
            dp_indices[idx] = i;
        }
        if (idx > 0) prev[i] = dp_indices[idx - 1];
    }
    lis = reconstruct_LIS(nums, prev, dp_indices.back());
    return dp.size();
}

int lowbit(int x) {
    return x & (-x);
}

class FenwickTree {
public:
    FenwickTree(int size) : tree(size + 1, 0) {}

    void update(int index, int value) {
        while (index < tree.size()) {
            tree[index] = max(tree[index], value);
            index += lowbit(index);
        }
    }

    int query(int index) {
        int maxVal = 0;
        while (index > 0) {
            maxVal = max(maxVal, tree[index]);
            index -= lowbit(index);
        }
        return maxVal;
    }

private:
    vector<int> tree;
};

int lengthOfLIS_BIT(vector<int>& nums, vector<int>& lis) {
    if (nums.empty()) return 0;

    vector<int> sortedNums = nums;
    sort(sortedNums.begin(), sortedNums.end());
    sortedNums.erase(unique(sortedNums.begin(), sortedNums.end()), sortedNums.end());

    FenwickTree fenwickTree(sortedNums.size());
    vector<int> dp(nums.size());
    unordered_map<int, int> indexMap;

    int lisLength = 0;
    int lastIndex = -1;

    for (int i = 0; i < nums.size(); ++i) {
        int index = lower_bound(sortedNums.begin(), sortedNums.end(), nums[i]) - sortedNums.begin() + 1;
        int maxLIS = fenwickTree.query(index - 1) + 1;
        dp[i] = maxLIS;
        fenwickTree.update(index, maxLIS);

        if (maxLIS > lisLength) {
            lisLength = maxLIS;
            lastIndex = i;
        }
        indexMap[maxLIS] = i;
    }

    vector<int> tempLIS(lisLength);
    for (int i = lisLength; i > 0; --i) {
        tempLIS[i - 1] = nums[lastIndex];
        lastIndex = indexMap[i - 1];
    }
    lis = tempLIS;
    return lisLength;
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
    vector<int> lis;
    cout << "Length of LIS (DP O(n^2)): " << lengthOfLIS_DP(nums, lis) << endl;
    cout << "LIS: ";
    for (int num : lis) cout << num << " ";
    cout << endl;

    lis.clear();
    cout << "Length of LIS (Greedy O(n log n)): " << lengthOfLIS_Greedy(nums, lis) << endl;
    cout << "LIS: ";
    for (int num : lis) cout << num << " ";
    cout << endl;

    lis.clear();
    cout << "Length of LIS (BIT O(n log n)): " << lengthOfLIS_BIT(nums, lis) << endl;
    cout << "LIS: ";
    for (int num : lis) cout << num << " ";
    cout << endl;

    return 0;
}
