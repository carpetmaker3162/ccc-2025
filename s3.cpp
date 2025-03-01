// author: ChatGPT o3

#include <bits/stdc++.h>
using namespace std;
 
typedef long long ll;
 
// A very-large constant used as “infinity” (for second–minimum computations)
const ll INF = 1e18;
// A very–negative constant for when a candidate does not exist.
const ll NEG_INF = -1000000000000000000LL;
 
// -----------------------------------------------------------------------------
// SEGMENT TREE FOR MINIMUM (to quickly get the global minimum “best” value
// and the second–minimum value, along with the colour index having that minimum)
// -----------------------------------------------------------------------------
struct NodeMin {
    ll minVal;      // the minimum value in this segment
    ll secondMin;   // the second minimum value in this segment
    int idx;        // index (i.e. colour) where minVal is attained
};
 
// Combine two NodeMin values from two segments.
NodeMin combine(const NodeMin &a, const NodeMin &b) {
    NodeMin res;
    if(a.minVal < b.minVal) {
        res.minVal = a.minVal;
        res.idx = a.idx;
        res.secondMin = min(a.secondMin, b.minVal);
    } else if(a.minVal > b.minVal) {
        res.minVal = b.minVal;
        res.idx = b.idx;
        res.secondMin = min(a.minVal, b.secondMin);
    } else {  // Equal minimums: pick one index arbitrarily and the second min is the min of both second mins.
        res.minVal = a.minVal;
        res.idx = a.idx;
        res.secondMin = min(a.secondMin, b.secondMin);
    }
    return res;
}
 
struct SegTreeMin {
    int n;
    vector<NodeMin> tree;
    SegTreeMin(int n_) {
        n = n_;
        tree.resize(4 * n);
    }
    // Build the tree over positions [l,r] (1-indexed)
    void build(vector<ll>& arr, int idx, int l, int r) {
        if(l == r) {
            tree[idx].minVal = arr[l];
            tree[idx].secondMin = INF;
            tree[idx].idx = l;
            return;
        }
        int mid = (l + r) / 2;
        build(arr, idx * 2, l, mid);
        build(arr, idx * 2 + 1, mid + 1, r);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1]);
    }
    void build(vector<ll>& arr) {
        build(arr, 1, 1, n);
    }
    // Update the value at position 'pos' to 'val'
    void update(int pos, ll val, int idx, int l, int r) {
        if(l == r) {
            tree[idx].minVal = val;
            tree[idx].secondMin = INF;
            tree[idx].idx = pos;
            return;
        }
        int mid = (l + r) / 2;
        if(pos <= mid)
            update(pos, val, idx * 2, l, mid);
        else
            update(pos, val, idx * 2 + 1, mid + 1, r);
        tree[idx] = combine(tree[idx * 2], tree[idx * 2 + 1]);
    }
    void update(int pos, ll val) {
        update(pos, val, 1, 1, n);
    }
    // The query for the entire range is just tree[1]
    NodeMin query() { return tree[1]; }
};
 
// -----------------------------------------------------------------------------
// SEGMENT TREE FOR MAXIMUM (to quickly obtain the maximum candidate value)
// -----------------------------------------------------------------------------
struct NodeMax {
    ll maxVal;
    int idx;
};
 
struct SegTreeMax {
    int n;
    vector<NodeMax> tree;
    SegTreeMax(int n_) {
        n = n_;
        tree.resize(4 * n);
    }
    // Build tree over [l,r] (1-indexed)
    void build(vector<ll>& arr, int idx, int l, int r) {
        if(l == r) {
            tree[idx].maxVal = arr[l];
            tree[idx].idx = l;
            return;
        }
        int mid = (l + r) / 2;
        build(arr, idx * 2, l, mid);
        build(arr, idx * 2 + 1, mid + 1, r);
        if(tree[idx * 2].maxVal >= tree[idx * 2 + 1].maxVal)
            tree[idx] = tree[idx * 2];
        else
            tree[idx] = tree[idx * 2 + 1];
    }
    void build(vector<ll>& arr) {
        build(arr, 1, 1, n);
    }
    // Update position pos to new value val.
    void update(int pos, ll val, int idx, int l, int r) {
        if(l == r) {
            tree[idx].maxVal = val;
            tree[idx].idx = pos;
            return;
        }
        int mid = (l + r) / 2;
        if(pos <= mid)
            update(pos, val, idx * 2, l, mid);
        else
            update(pos, val, idx * 2 + 1, mid + 1, r);
        if(tree[idx * 2].maxVal >= tree[idx * 2 + 1].maxVal)
            tree[idx] = tree[idx * 2];
        else
            tree[idx] = tree[idx * 2 + 1];
    }
    void update(int pos, ll val) { update(pos, val, 1, 1, n); }
    // Query the entire range:
    NodeMax query() { return tree[1]; }
    // Query maximum in an arbitrary range [ql,qr]
    NodeMax queryRange(int idx, int l, int r, int ql, int qr) {
        if(ql > r || qr < l) {
            NodeMax dummy;
            dummy.maxVal = NEG_INF;
            dummy.idx = -1;
            return dummy;
        }
        if(ql <= l && r <= qr)
            return tree[idx];
        int mid = (l + r) / 2;
        NodeMax leftNode = queryRange(idx * 2, l, mid, ql, qr);
        NodeMax rightNode = queryRange(idx * 2 + 1, mid + 1, r, ql, qr);
        return (leftNode.maxVal >= rightNode.maxVal) ? leftNode : rightNode;
    }
    NodeMax queryRange(int l, int r) { return queryRange(1, 1, n, l, r); }
};
 
// -----------------------------------------------------------------------------
// GLOBAL VARIABLES & DATA STRUCTURES
// -----------------------------------------------------------------------------
int N, M, Q;
 
// For each pen (1-indexed), store its current color and prettiness.
vector<int> penColor;  // penColor[i] = current color of pen i
vector<ll> penVal;     // penVal[i] = current prettiness of pen i
 
// For each color (1-indexed), we store all the pens’ prettiness in a multiset.
vector<multiset<ll>> colorSets;
 
// For each color, we keep two values:
//   bestArr[c]    = maximum prettiness in color c
//   candArr[c]    = candidate value (if best is unique then the second highest, otherwise best)
vector<ll> bestArr;  
vector<ll> candArr;
 
// baseSum = sum_{c=1}^{M} (bestArr[c])
ll baseSum = 0;
 
// We'll build two segment trees over colors 1..M.
// segMin is built on bestArr (to quickly get the global minimum best and its second–min)
// segMax is built on candArr (to quickly get the maximum candidate)
SegTreeMin *segMin;
SegTreeMax *segMax;
 
// -----------------------------------------------------------------------------
// UPDATE FUNCTION FOR A SINGLE COLOR
// When a pen is removed from or added to a colour, update that colour's best/candidate,
// update the baseSum, and update the segment trees.
void updateColor(int c) {
    ll oldBest = bestArr[c];
    // new best is the maximum in the multiset for color c.
    ll newBest = *colorSets[c].rbegin();
    bestArr[c] = newBest;
    // Compute candidate: if newBest occurs at least twice, candidate = newBest;
    // otherwise, candidate = second highest (or NEG_INF if none exists – dummy -1 is always present).
    int freq = colorSets[c].count(newBest);
    ll newCand;
    if(freq >= 2)
        newCand = newBest;
    else {
        auto it = colorSets[c].rbegin();
        ++it;
        if(it != colorSets[c].rend())
            newCand = *it;
        else
            newCand = NEG_INF;
    }
    candArr[c] = newCand;
    // Update base sum with the difference between the new and old best values.
    baseSum += (newBest - oldBest);
    // Update the segment trees for this color.
    segMin->update(c, newBest);
    segMax->update(c, newCand);
}
 
// -----------------------------------------------------------------------------
// COMPUTE THE FINAL ANSWER
// The answer equals baseSum + extraGain, where extraGain is computed as follows:
//
// Let g be the smallest best value over all colors (the “bottleneck”) and let
// g2 be the second–smallest best (if g is unique) or g if not.
// Also, let option1 = candidate[arg] – g2, where arg is the color with g,
// and option2 = (maximum candidate among other colors) – g.
// Then extraGain = max(0, max(option1, option2)).
ll computeAnswer() {
    // Query segMin to get the global minimum best and its color index.
    NodeMin node = segMin->query();
    ll g = node.minVal;
    int arg = node.idx;
    ll g2 = node.secondMin;
    // Option 1: if we change the pen from the bottleneck color.
    ll option1 = candArr[arg] - g2;
    // Option 2: the maximum candidate from colors other than arg.
    NodeMax maxNode = segMax->query();
    ll option2 = 0;
    if(maxNode.idx == arg) {
        // If the maximum candidate comes from the bottleneck color, search in the remainder.
        if(arg > 1) {
            NodeMax leftNode = segMax->queryRange(1, arg - 1);
            option2 = leftNode.maxVal - g;
        }
        if(arg < M) {
            NodeMax rightNode = segMax->queryRange(arg + 1, M);
            option2 = max(option2, rightNode.maxVal - g);
        }
    } else {
        option2 = maxNode.maxVal - g;
    }
    ll extra = max({option1, option2, 0LL});
    return baseSum + extra;
}
 
// -----------------------------------------------------------------------------
// MAIN FUNCTION
// -----------------------------------------------------------------------------
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
 
    cin >> N >> M >> Q;
    penColor.resize(N + 1);
    penVal.resize(N + 1);
    colorSets.resize(M + 1);
    bestArr.resize(M + 1);
    candArr.resize(M + 1);
 
    // Read the initial pens.
    for (int i = 1; i <= N; i++){
        int c;
        ll p;
        cin >> c >> p;
        penColor[i] = c;
        penVal[i] = p;
        colorSets[c].insert(p);
    }
 
    // For each color, ensure there is a dummy value (-1) so that there is always a second element.
    // (The problem guarantees at least one pen per color, but the dummy simplifies candidate computations.)
    for (int c = 1; c <= M; c++){
        colorSets[c].insert(-1);
        // Compute best and candidate for this color.
        ll bestVal = *colorSets[c].rbegin();
        bestArr[c] = bestVal;
        baseSum += bestVal;
        int freq = colorSets[c].count(bestVal);
        ll cand;
        if(freq >= 2)
            cand = bestVal;
        else {
            auto it = colorSets[c].rbegin();
            ++it;
            if(it != colorSets[c].rend())
                cand = *it;
            else
                cand = NEG_INF;
        }
        candArr[c] = cand;
    }
 
    // Build two arrays (indexed 1..M) for the segment trees.
    vector<ll> bestArray(M + 1), candArray(M + 1);
    for (int c = 1; c <= M; c++){
        bestArray[c] = bestArr[c];
        candArray[c] = candArr[c];
    }
    // Build the segment trees.
    segMin = new SegTreeMin(M);
    segMin->build(bestArray);
    segMax = new SegTreeMax(M);
    segMax->build(candArray);
 
    // Print the answer for the initial state.
    cout << computeAnswer() << "\n";
 
    // Process Q queries.
    for (int qi = 0; qi < Q; qi++){
        int t, i;
        cin >> t >> i;
        int oldColor = penColor[i];
        ll oldVal = penVal[i];
        if(t == 1) {
            // Query type 1: change the color of pen i.
            int newColor;
            cin >> newColor;
            penColor[i] = newColor;
            // Remove pen i from its old color.
            auto it = colorSets[oldColor].find(oldVal);
            if(it != colorSets[oldColor].end())
                colorSets[oldColor].erase(it);
            // Update the old color.
            updateColor(oldColor);
 
            // Insert pen i into the new color.
            colorSets[newColor].insert(oldVal);
            updateColor(newColor);
        } else {
            // Query type 2: change the prettiness of pen i.
            ll newVal;
            cin >> newVal;
            penVal[i] = newVal;
            int c = penColor[i];
            auto it = colorSets[c].find(oldVal);
            if(it != colorSets[c].end())
                colorSets[c].erase(it);
            colorSets[c].insert(newVal);
            updateColor(c);
        }
 
        // Output the answer after processing this query.
        cout << computeAnswer() << "\n";
    }
 
    return 0;
}
