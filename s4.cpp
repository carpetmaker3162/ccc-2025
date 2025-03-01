// Author: ChatGPT o3

#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
 
struct Edge {
    int to;
    ll w;
};
 
// Structure for tunnels
struct Tunnel {
    int u, v;
    ll c;
};
 
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
 
    int N, M;
    cin >> N >> M;
    
    // Read tunnel data and collect temperatures per room
    vector<Tunnel> tunnels(M);
    vector<vector<ll>> roomTemps(N+1);
    
    for (int i = 0; i < M; i++){
        int u, v;
        ll c;
        cin >> u >> v >> c;
        tunnels[i] = {u, v, c};
        roomTemps[u].push_back(c);
        roomTemps[v].push_back(c);
    }
    // In room 1, add initial chill level 0.
    roomTemps[1].push_back(0);
    
    // Remove duplicates and sort for each room.
    for (int i = 1; i <= N; i++){
        sort(roomTemps[i].begin(), roomTemps[i].end());
        roomTemps[i].erase(unique(roomTemps[i].begin(), roomTemps[i].end()), roomTemps[i].end());
    }
    
    // Build a mapping: assign each (room, chill level) a unique state id.
    vector<int> startIndex(N+1, 0);
    int totalStates = 0;
    for (int i = 1; i <= N; i++){
        startIndex[i] = totalStates;
        totalStates += roomTemps[i].size();
    }
    
    // Build the graph.
    vector<vector<Edge>> graph(totalStates);
    
    // Internal moves within each room: connect adjacent chill levels.
    for (int i = 1; i <= N; i++){
        int base = startIndex[i];
        int sz = roomTemps[i].size();
        for (int j = 0; j < sz - 1; j++){
            ll diff = roomTemps[i][j+1] - roomTemps[i][j];
            int u = base + j;
            int v = base + j + 1;
            graph[u].push_back({v, diff});
            graph[v].push_back({u, diff});
        }
    }
    
    // Tunnel transitions: if a tunnel from u to v has temperature c,
    // then from state (u, c) you can jump to state (v, c) at cost 0.
    for (int i = 0; i < M; i++){
        int u = tunnels[i].u, v = tunnels[i].v;
        ll c = tunnels[i].c;
        int posU = int(lower_bound(roomTemps[u].begin(), roomTemps[u].end(), c) - roomTemps[u].begin());
        int posV = int(lower_bound(roomTemps[v].begin(), roomTemps[v].end(), c) - roomTemps[v].begin());
        int stateU = startIndex[u] + posU;
        int stateV = startIndex[v] + posV;
        graph[stateU].push_back({stateV, 0});
        graph[stateV].push_back({stateU, 0});
    }
    
    // Run Dijkstra from the starting state: room 1, chill level 0.
    const ll INF = 1LL << 60;
    vector<ll> dist(totalStates, INF);
    
    int startState = startIndex[1] + int(lower_bound(roomTemps[1].begin(), roomTemps[1].end(), 0) - roomTemps[1].begin());
    dist[startState] = 0;
    
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
    pq.push({0, startState});
    
    while(!pq.empty()){
        auto [d, cur] = pq.top();
        pq.pop();
        if(d != dist[cur]) continue;
        for(auto &edge : graph[cur]){
            int nxt = edge.to;
            ll nd = d + edge.w;
            if(nd < dist[nxt]){
                dist[nxt] = nd;
                pq.push({nd, nxt});
            }
        }
    }
    
    // Answer is the minimum cost to reach any state in room N.
    ll ans = INF;
    int base = startIndex[N];
    int sz = roomTemps[N].size();
    for (int i = 0; i < sz; i++){
        ans = min(ans, dist[base + i]);
    }
    cout << ans << "\n";
    return 0;
}

