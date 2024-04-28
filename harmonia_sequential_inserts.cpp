#include <bits/stdc++.h>
#define MAX 1000000000
using namespace std;

int k1 = 364;
int k2 = 121;   // k1= 3^0+...+3^5 = 364= sizeof(nodes)
                //  k2=364-3^5= sizeof(prefix_sum)
int m = 101;    // 1 key + 4 data items
int fanout = 6; // so, each node can have max 3 keys and 4 children
int nk = fanout - 1;

bool flag = false;
int incremented_index = -1;

typedef struct Node
{
  int *key; // keys are sorted
  int **data;
} node;

node **nodes = (node **)malloc(sizeof(node *) * k1);
int *prefix_sum = (int *)malloc(sizeof(int) * k2);

int lb(int *arr, int N, int X)
{
  int mid;

  // Initialise starting index and
  // ending index
  int low = 0;
  int high = N;

  // Till low is less than high
  while (low < high)
  {
    mid = low + (high - low) / 2;

    // If X is less than or equal
    // to arr[mid], then find in
    // left subarray
    if (X <= arr[mid])
    {
      high = mid;
    }

    // If X is greater arr[mid]
    // then find in right subarray
    else
    {
      low = mid + 1;
    }
  }

  // Return the lb index
  return low;
}

void init(node *nn)
{
  nn->key = (int *)malloc(sizeof(int) * nk);     // init each node with 3 keys
  nn->data = (int **)malloc(sizeof(int *) * nk); // and 3 data arrays, each having m integers
  for (int i = 0; i < nk; i++)
  {
    nn->data[i] = (int *)malloc(sizeof(int) * m);
  }

  for (int i = 0; i < nk; i++)
    nn->key[i] = MAX;

  for (int i = 0; i < nk; i++)
  {
    for (int j = 0; j < m; j++)
    {
      nn->data[i][j] = MAX;
    }
  }
}

int node_count = 0;
int prefix_sum_count = 0;

int search(int key)
{

  int i = 0; // node* ptr; ptr=nodes[0];
  int j = 0;

  while (true)
  {
    // ptr=nodes[i];
    j = i;
    if (nodes[i] == NULL) // no node inserted yet
      return 0;
    int *key_arr = nodes[i]->key;
    int ind = lb(key_arr, nk, key);
    // cout << "i: " << i << "; ind: " << ind << " ; prefix_sum[i]: " << prefix_sum[i] << endl;
    if (prefix_sum[i] == MAX) // reached a leaf
      break;
    i = prefix_sum[i] + ind;
    if (key_arr[ind] == key)
      i++;
  }

  int *key_arr = nodes[j]->key;
  int ind = lb(key_arr, nk, key);
  // cout<<"SEARCH"<<endl;
  bool ok = false;
  if (key_arr[ind] == key)
  {
    int **dd = nodes[j]->data;
    for (int r = 0; r < m; r++)
      cout << dd[ind][r] << "\n";
    // cout << dd[ind][1] << "\n";
    ok = true;
  }
  if (!ok)
    cout << "-1\n";
  return j;
}

void range_query(int k1, int k2) // scan leaves from first key to last key
{
  int j = search(k1);
  int *key_arr = nodes[j]->key;
  int ind = lb(key_arr, nk, k1);
  // cout<<"SEARCH"<<endl;
  cout << "range_query: " << endl;
  if (key_arr[ind] == k1)
  {
    cout << "In" << endl;
    ind++;
    int **dd = nodes[j]->data;

    for (; ind < nk; ind++)
    {
      if (key_arr[ind] == MAX)
        break;
      for (int r = 0; r < m; r++)
        cout << dd[ind][r] << " ";
    }
    cout << endl;
  }

  j++;
  bool f2 = false;

  while (nodes[j] != NULL && f2 == false)
  {
    int *key_arr = nodes[j]->key;
    int **dd = nodes[j]->data;
    int ind = 0;
    for (; ind < nk; ind++)
    {
      if (key_arr[ind] == MAX)
        break;
      if (dd[ind][0] > k2)
      {
        f2 = true;
        break;
      }
      for (int r = 0; r < m; r++)
        cout << dd[ind][r] << " ";
      cout << endl;
    }
    j++;
  }
}

void put_in_middle(node *nn, int pos, int key, int *dd) // put key and its dd at position pos in nn
{
  int arr[nk];
  int dt[nk][m];
  for (int i = 0; i < nk; i++)
    arr[i] = nn->key[i];

  for (int i = 0; i < nk; i++)
  {
    for (int j = 0; j < m; j++)
    {
      dt[i][j] = nn->data[i][j];
    }
  }

  nn->key[pos] = key;
  for (int i = pos + 1; i < nk; i++)
    nn->key[i] = arr[i - 1];

  for (int j = 0; j < m; j++)
    nn->data[pos][j] = dd[j];

  for (int i = pos + 1; i < nk; i++)
  {
    for (int j = 0; j < m; j++)
    {
      nn->data[i][j] = dt[i - 1][j];
    }
  }
}

void put_in_nodes(int ind, node *nn2) // put nn2 in nodes at ind position
{
  node *g = nodes[ind];
  nodes[ind] = nn2;
  int i = ind + 1;
  for (i = ind + 1; i < k1 && nodes[i] != NULL; i++)
  {
    node *h = nodes[i];
    nodes[i] = g;
    g = h;
  }
  nodes[i] = g;
}

void put_in_array(int *arr, int sz, int x, int ind) // put value x at index ind in array arr. sz is size of array
{
  int g = arr[ind];
  arr[ind] = x;
  int i = ind + 1;
  for (i = ind + 1; i < k1 && arr[i] != MAX; i++)
  {
    int h = arr[i];
    arr[i] = g;
    g = h;
  }
  arr[i] = g;
}

int parent(int ind) // give index of parent of node ind
{
  if (prefix_sum[0] == MAX || ind == 0) // no element in prefix_sum
    return -1;

  int p = lb(prefix_sum, k2, ind);
  // cout<<"pp: "<<p<<"; prefix_sum[p]: "<<prefix_sum[p]<<endl;
  if (prefix_sum[p] != ind)
    p--;
  return p;
}

int insert_internal(node *nn, int ind, int key, int *dd, int orig) // insert key in node* nn
{
  cout << "start ind: " << ind << "; start key: " << key << endl;
  cout << "nk: " << nk << endl;
  cout << "Keys of start node nn: " << endl;

  for (int i = 0; i < nk; i++)
    cout << nn->key[i] << " ";
  cout << endl;

  int pos = lb(nn->key, nk, key);
  if (nn->key[nk - 1] == MAX) // space there in node
  {
    cout << "Inserting normally" << endl;
    put_in_middle(nn, pos, key, dd);
    return 0;
  }
  else // node is full
  {
    cout << "Node is full" << endl;
    for (int i = 0; i < nk; i++)
    {
      cout << nn->key[i] << "# " << endl;
    }
    cout << "key: " << key << endl;

    int pos = lb(nn->key, nk, key);
    node *nn2;
    int *dd_mid;
    dd_mid = (int *)malloc(m * sizeof(int));
    int middle_element; // middle_element goes to top next time
    cout << "pos: " << pos << endl;

    if (prefix_sum[ind] == MAX) // leaf
    {
      int mid = (nk + 1) / 2;
      cout << "mid: " << mid << endl;

      if (pos < mid)
      {
        nn2 = (node *)malloc(sizeof(node));
        init(nn2);
        node_count++;
        middle_element = nn->key[mid - 1]; // mid-1 goes to top
        for (int j = 0; j < m; j++)
          dd_mid[j] = nn->data[mid - 1][j];

        for (int i = mid - 1; i < nk; i++)
        {
          nn2->key[i - (mid - 1)] = nn->key[i];
        }

        for (int i = mid - 1; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn2->data[i - (mid - 1)][j] = nn->data[i][j];
          }
        }

        for (int i = mid - 1; i < nk; i++)
        {
          nn->key[i] = MAX;
        }
        for (int i = mid - 1; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn->data[i][j] = MAX;
          }
        }

        put_in_middle(nn, pos, key, dd);
      }
      else
      {
        if (pos == mid)
        {
          middle_element = key;
          for (int j = 0; j < m; j++)
            dd_mid[j] = dd[j];
        }
        else
        {
          middle_element = nn->key[mid];
          cout << "middle_element: " << middle_element << endl;
          for (int j = 0; j < m; j++)
          {
            cout << "j: " << j << endl;
            dd_mid[j] = nn->data[mid][j];
          }
        }

        nn2 = (node *)malloc(sizeof(node));
        init(nn2);
        node_count++;

        for (int i = mid; i < nk; i++)
        {
          nn2->key[i - mid] = nn->key[i];
        }
        for (int i = mid; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn2->data[i - mid][j] = nn->data[i][j];
          }
        }
        put_in_middle(nn2, pos - mid, key, dd);

        cout << "mid2: " << mid << endl;
        for (int i = mid; i < nk; i++)
        {
          cout << "i: " << i << endl;
          nn->key[i] = MAX;
        }

        for (int i = mid; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn->data[i][j] = MAX;
          }
        }
      }
    }
    else
    {
      int mid = (nk + 1) / 2;

      if (pos < mid)
      {
        nn2 = (node *)malloc(sizeof(node));
        init(nn2);
        node_count++;

        middle_element = nn->key[mid - 1]; // mid-1 goes to top
        for (int j = 0; j < m; j++)
          dd_mid[j] = nn->data[mid - 1][j];

        for (int i = mid; i < nk; i++) // leaving mid-1
        {
          nn2->key[i - (mid)] = nn->key[i];
        }

        for (int i = mid; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn2->data[i - (mid)][j] = nn->data[i][j];
          }
        }

        for (int i = mid - 1; i < nk; i++)
        {
          nn->key[i] = MAX;
        }
        for (int i = mid - 1; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn->data[i][j] = MAX;
          }
        }

        put_in_middle(nn, pos, key, dd);
      }
      else
      {
        if (pos == mid)
        {
          middle_element = key;
          for (int j = 0; j < m; j++)
            dd_mid[j] = dd[j];
          nn2 = (node *)malloc(sizeof(node));
          init(nn2);
          node_count++;

          for (int i = mid; i < nk; i++)
          {
            nn2->key[i - mid] = nn->key[i];
          }
          for (int i = mid; i < nk; i++)
          {
            for (int j = 0; j < m; j++)
            {
              nn2->data[i - mid][j] = nn->data[i][j];
            }
          }
        }
        else
        {
          middle_element = nn->key[mid];
          for (int j = 0; j < m; j++)
            dd_mid[j] = nn->data[mid][j];
          nn2 = (node *)malloc(sizeof(node));
          init(nn2);
          node_count++;

          for (int i = mid + 1; i < nk; i++)
          {
            nn2->key[i - (mid + 1)] = nn->key[i];
          }
          for (int i = mid + 1; i < nk; i++)
          {
            for (int j = 0; j < m; j++)
            {
              nn2->data[i - (mid + 1)][j] = nn->data[i][j];
            }
          }
          put_in_middle(nn2, pos - (mid + 1), key, dd);
        }

        for (int i = mid; i < nk; i++)
        {
          nn->key[i] = MAX;
        }
        for (int i = mid; i < nk; i++)
        {
          for (int j = 0; j < m; j++)
          {
            nn->data[i][j] = MAX;
          }
        }
      }
    }

    put_in_nodes(ind + 1, nn2);

    if (prefix_sum[ind] == MAX) // leaf
    {
      int xx = 0;
      int p = parent(ind);
      if (p == -1 || nodes[p] == NULL) // leaf and parent is null means first node to be filled, and now, prefix_sum will get its first entry
      {
        cout << endl;
        cout << "Leaf with parent NULL:- ind: " << ind << "; p: " << p << endl;
        prefix_sum[0] = 1;
        node *nn3;
        nn3 = (node *)malloc(sizeof(node));
        init(nn3);
        node_count++;

        // cout<<"middle_element: "<<middle_element<<endl;
        put_in_middle(nn3, 0, middle_element, dd_mid);
        // cout<<"middle_element: "<<middle_element<<endl;
        put_in_nodes(0, nn3);
        return 0;
      }
      else
        xx = insert_internal(nodes[p], p, middle_element, dd_mid, ind); // dd is NULL means we won't insert anything

      ind += xx;
      cout << "Leaf with parent non-null:- ind+=xx: " << ind << "; p: " << p << endl;

      cout << endl;
      p = parent(ind);
      int ind2 = prefix_sum[p];
      cout << "prefix_sum: " << endl;
      for (int i = 0; i < k2 && prefix_sum[i] != MAX; i++)
      {
        cout << prefix_sum[i] << " ";
      }
      cout << endl;

      for (int i = 0; i < k1; i++)
      {
        if (nodes[i] != NULL)
        {
          cout << "nodes[" << i << "]->key: " << endl;
          for (int j = 0; j < nk; j++)
          {
            cout << (nodes[i]->key)[j] << " ";
          }
          cout << endl;
          cout << "nodes[" << i << "]->data: " << endl;
          for (int j = 0; j < nk; j++)
          {
            cout << "data[" << j << "]- " << endl;
            for (int f = 0; f < m; f++)
            {
              cout << (nodes[i]->data)[j][f] << " ";
            }
            cout << endl;
          }
          cout << endl;
        }
      }

      cout << "p: " << p << " ; ind: " << ind << "; ind2: " << ind2 << endl;

      cout << "prefix_sum in full leaf whose parent is not null, before: " << endl;
      for (int i = 0; i < k2 && prefix_sum[i] != MAX; i++)
      {
        cout << prefix_sum[i] << " ";
      }
      cout << endl;

      // if(flag==false)
      //{
      cout << "incremented_index: " << incremented_index << endl;
      for (int i = 0; i < k2; i++)
      {
        if (prefix_sum[i] == MAX)
          break;

        if (prefix_sum[i] > ind2)
        {
          if (!(prefix_sum[i] == incremented_index && flag == true))
            prefix_sum[i] += 1;
        }
      }
      //}

      cout << "prefix_sum in full leaf whose parent is not null,after : " << endl;
      for (int i = 0; i < k2 && prefix_sum[i] != MAX; i++)
      {
        cout << prefix_sum[i] << " ";
      }
      cout << endl;

      return xx + 1;
    }
    else // non-leaf
    {
      int x = 0;
      int no_of_keys_of_v1 = 0;
      int xx = 0;
      for (int i = 0; i < nk; i++)
      {
        if (nn->key[i] != MAX)
          no_of_keys_of_v1++;
        else
          break;
      }

      int p = parent(ind);
      if (p == -1 || nodes[p] == NULL) // parent is NULL
      {
        cout << endl;
        cout << "In non-leaf with parent NULL, ind: " << ind << "; parent(ind): " << p << endl;
        put_in_array(prefix_sum, k2, 1, 0);
        x = 2;

        node *nn3;
        nn3 = (node *)malloc(sizeof(node));
        init(nn3);
        node_count++;

        // cout<<"middle_element: "<<middle_element<<endl;
        put_in_middle(nn3, 0, middle_element, dd_mid);
        // cout<<"middle_element: "<<middle_element<<endl;
        put_in_nodes(0, nn3);

        int pf_2 = -1;
        int ss = prefix_sum[1] + 2;
        cout << "ss: " << ss << " ; orig+2: " << orig + 2 << "; ind: " << ind << endl;
        if (ss > orig + 2)
        {
          pf_2 = ss - 2 + no_of_keys_of_v1;
          flag = false;
        }
        else
        {
          pf_2 = ss - 2 + no_of_keys_of_v1 + 1;
          flag = true;
        }

        cout << "pf_2: " << pf_2 << endl;
        incremented_index = pf_2 + 2;
        ;
        cout << "incremented_index in orig+2: " << incremented_index << endl;
        put_in_array(prefix_sum, k2, pf_2, 2);

        cout << "prefix_sum before " << endl;
        for (int i = 0; i < k2 && prefix_sum[i] != MAX; i++)
        {
          cout << prefix_sum[i] << " ";
        }
        cout << endl;

        for (int i = 1; i < k2; i++)
        {
          if (prefix_sum[i] == MAX)
            break;

          if (prefix_sum[i] > ind)
            prefix_sum[i] += x;
        }

        cout << "prefix_sum after: " << endl;
        for (int i = 0; i < k2 && prefix_sum[i] != MAX; i++)
        {
          cout << prefix_sum[i] << " ";
        }
        cout << endl;

        return 2;
      }
      else
      {
        int p = parent(ind);
        int xx = insert_internal(nodes[p], p, middle_element, dd_mid, ind);
        ind += xx;
        x = 1;
      }
      cout << endl;
      cout << "In non-leaf with parent non-null, ind+=xx: " << ind << "; parent(ind): " << p << endl;

      for (int i = 0; i < k2; i++)
      {
        if (prefix_sum[i] == MAX)
          break;

        if (prefix_sum[i] > ind)
          prefix_sum[i] += x;
      }

      int pf_2 = -1;
      int ss = prefix_sum[ind];
      cout << "ss: " << ss << " ; orig+xx+1: " << orig + xx + 1 << "; ind: " << ind << "; xx: " << xx << "; orig: " << orig << endl;
      if (ss > orig + xx + 1)
      {
        pf_2 = ss + no_of_keys_of_v1;
        flag = false;
      }
      else
      {
        pf_2 = ss + no_of_keys_of_v1 + 1;
        flag = true;
      }

      cout << "pf_2: " << pf_2 << endl;
      incremented_index = pf_2;
      cout << "incremented_index in orig+ind: " << incremented_index << endl;
      put_in_array(prefix_sum, k2, pf_2, ind + 1);

      return (xx + 1);
    }
  }
}
// ---- DELETION ----
bool checkKeyExists(int key)
{
  if (node_count == 0)
    return false;
  int j = search(key);
  int *key_arr = nodes[j]->key;
  int ind = lb(key_arr, nk, key);
  int d[1][1] = {{MAX}};
  int **dd = (int **)d;
  if (key_arr[ind] == key)
  {
    dd = nodes[j]->data;
    for (int r = 0; r < m; r++)
      dd[ind][r] = MAX;
  }
  return dd[0][0] != MAX;
}
int order = 3;
typedef struct record
{
  int value;
} record;
typedef struct node2
{
  void **pointers;
  int *keys;
  struct node2 *parent;
  bool is_leaf;
  int num_keys;
  struct node2 *next;
} node2;

node2 *remove_entry_from_node(node2 *n, int key, void *pointer)
{
  int i, num_pointers;
  i = 0;
  while (n->keys[i] != key)
    i++;
  for (++i; i < n->num_keys; i++)
    n->keys[i - 1] = n->keys[i];

  num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
  i = 0;
  while (n->pointers[i] != pointer)
    i++;
  for (++i; i < num_pointers; i++)
    n->pointers[i - 1] = n->pointers[i];

  n->num_keys--;

  if (n->is_leaf)
    for (i = n->num_keys; i < order - 1; i++)
      n->pointers[i] = NULL;
  else
    for (i = n->num_keys + 1; i < order; i++)
      n->pointers[i] = NULL;

  return n;
}
node2 *adjust_root(node2 *root)
{
  void *new_root;

  if (root->num_keys > 0)
    return root;

  if (!root->is_leaf)
  {
    new_root = root->pointers[0];
    // new_root->parent = NULL;
  }

  else
    new_root = NULL;

  free(root->keys);
  free(root->pointers);
  free(root);
  node2 *new_root2 = new node2();
  return new_root2;
}
int cut(int length)
{
  if (length % 2 == 0)
    return length / 2;
  else
    return length / 2 + 1;
}

node2 *redistribute_nodes(node2 *root, node2 *n, node2 *neighbor, int neighbor_index,
                          int k_prime_index, int k_prime)
{
  int i;
  node2 *tmp;

  if (neighbor_index != -1)
  {
    if (!n->is_leaf)
      n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
    for (i = n->num_keys; i > 0; i--)
    {
      n->keys[i] = n->keys[i - 1];
      n->pointers[i] = n->pointers[i - 1];
    }
    if (!n->is_leaf)
    {
      n->pointers[0] = neighbor->pointers[neighbor->num_keys];
      tmp = (node2 *)n->pointers[0];
      tmp->parent = n;
      neighbor->pointers[neighbor->num_keys] = NULL;
      n->keys[0] = k_prime;
      n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
    }
    else
    {
      n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
      neighbor->pointers[neighbor->num_keys - 1] = NULL;
      n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
      n->parent->keys[k_prime_index] = n->keys[0];
    }
  }

  else
  {
    if (n->is_leaf)
    {
      n->keys[n->num_keys] = neighbor->keys[0];
      n->pointers[n->num_keys] = neighbor->pointers[0];
      n->parent->keys[k_prime_index] = neighbor->keys[1];
    }
    else
    {
      n->keys[n->num_keys] = k_prime;
      n->pointers[n->num_keys + 1] = neighbor->pointers[0];
      tmp = (node2 *)n->pointers[n->num_keys + 1];
      tmp->parent = n;
      n->parent->keys[k_prime_index] = neighbor->keys[0];
    }
    for (i = 0; i < neighbor->num_keys - 1; i++)
    {
      neighbor->keys[i] = neighbor->keys[i + 1];
      neighbor->pointers[i] = neighbor->pointers[i + 1];
    }
    if (!n->is_leaf)
      neighbor->pointers[i] = neighbor->pointers[i + 1];
  }

  n->num_keys++;
  neighbor->num_keys--;

  return root;
}

int get_neighbor_index(node2 *n)
{
  int i;
  for (i = 0; i <= n->parent->num_keys; i++)
    if (n->parent->pointers[i] == n)
      return i - 1;

  printf("Search for nonexistent pointer to node in parent.\n");
  printf("Node:  %#lx\n", (unsigned long)n);
  exit(EXIT_FAILURE);
}

node2 *coalesce_nodes(node2 *root, node2 *n, node2 *neighbor, int neighbor_index, int k_prime)
{
  int i, j, neighbor_insertion_index, n_end;
  node2 *tmp;

  if (neighbor_index == -1)
  {
    tmp = n;
    n = neighbor;
    neighbor = tmp;
  }

  neighbor_insertion_index = neighbor->num_keys;

  if (!n->is_leaf)
  {
    neighbor->keys[neighbor_insertion_index] = k_prime;
    neighbor->num_keys++;

    n_end = n->num_keys;

    for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++)
    {
      neighbor->keys[i] = n->keys[j];
      neighbor->pointers[i] = n->pointers[j];
      neighbor->num_keys++;
      n->num_keys--;
    }

    neighbor->pointers[i] = n->pointers[j];

    for (i = 0; i < neighbor->num_keys + 1; i++)
    {
      tmp = (node2 *)neighbor->pointers[i];
      tmp->parent = neighbor;
    }
  }

  else
  {
    for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++)
    {
      neighbor->keys[i] = n->keys[j];
      neighbor->pointers[i] = n->pointers[j];
      neighbor->num_keys++;
    }
    neighbor->pointers[order - 1] = n->pointers[order - 1];
  }

  // root = delete_entry(root, n->parent, k_prime, n);
  free(n->keys);
  free(n->pointers);
  free(n);
  return root;
}

node2 *delete_entry(node2 *root, node2 *n, int key, void *pointer)
{
  int min_keys;
  node2 *neighbor;
  int neighbor_index;
  int k_prime_index, k_prime;
  int capacity;

  n = remove_entry_from_node(n, key, pointer);

  if (n == root)
    return adjust_root(root);

  min_keys = n->is_leaf ? cut(order - 1) : cut(order) - 1;

  if (n->num_keys >= min_keys)
    return root;

  neighbor_index = get_neighbor_index(n);
  k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
  k_prime = n->parent->keys[k_prime_index];
  // neighbor = neighbor_index == -1 ? n->parent->pointers[1] : n->parent->pointers[neighbor_index];
  neighbor = new node2();
  capacity = n->is_leaf ? order : order - 1;

  if (neighbor->num_keys + n->num_keys < capacity)
    return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);
  else
    return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}

node2 *find_leaf(node2 *const root, int key, bool verbose)
{
  if (root == NULL)
  {
    if (verbose)
      printf("Empty tree.\n");
    return root;
  }
  int i = 0;
  node2 *c = root;
  while (!c->is_leaf)
  {
    if (verbose)
    {
      printf("[");
      for (i = 0; i < c->num_keys - 1; i++)
        printf("%d ", c->keys[i]);
      printf("%d] ", c->keys[i]);
    }
    i = 0;
    while (i < c->num_keys)
    {
      if (key >= c->keys[i])
        i++;
      else
        break;
    }
    if (verbose)
      printf("%d ->\n", i);
    c = (node2 *)c->pointers[i];
  }
  if (verbose)
  {
    printf("Leaf [");
    for (i = 0; i < c->num_keys - 1; i++)
      printf("%d ", c->keys[i]);
    printf("%d] ->\n", c->keys[i]);
  }
  return c;
}

record *find(node2 *root, int key, bool verbose, node2 **leaf_out)
{
  if (root == NULL)
  {
    if (leaf_out != NULL)
    {
      *leaf_out = NULL;
    }
    return NULL;
  }

  int i = 0;
  node2 *leaf = NULL;

  leaf = find_leaf(root, key, verbose);

  for (i = 0; i < leaf->num_keys; i++)
    if (leaf->keys[i] == key)
      break;
  if (leaf_out != NULL)
  {
    *leaf_out = leaf;
  }
  if (i == leaf->num_keys)
    return NULL;
  else
    return (record *)leaf->pointers[i];
}

// ---------
void deleteKey(int key)
{
  bool keyExists = checkKeyExists(key);
  if (keyExists == false)
    return;
  node2 *key_leaf = NULL;
  record *key_record = NULL;
  node2 *root = NULL;
  key_record = find(root, key, false, &key_leaf);

  if (key_record != NULL && key_leaf != NULL)
  {
    root = delete_entry(root, key_leaf, key, key_record);
    // free(key_record);
  }
  // return root;
}

void update(int *arr)
{
  int key = arr[0];
  int j = search(key);
  int *key_arr = nodes[j]->key;
  int ind = lb(key_arr, nk, key);
  bool ok = false;
  if (key_arr[ind] == key)
  {
    int **dd = nodes[j]->data;
    if (dd[ind][0] != MAX)
    {
      for (int r = 0; r < m; r++)
        dd[ind][r] = arr[r];
      ok = true;
    }
  }
  cout << ok << "\n";
}

void insert(int *dd)
{
  int key = dd[0];
  int ind = search(key); // gives index of leaf in nodes[], where key can be inserted
  node *nn = nodes[ind];

  if (nn == NULL) // nodes[0] is NULL
  {
    nn = (node *)malloc(sizeof(node));
    init(nn);
    node_count++;

    nn->key[0] = key;
    for (int j = 0; j < m; j++)
    {
      nn->data[0][j] = dd[j];
    }

    nodes[0] = nn;
  }
  else // we have the leaf node nn to which original function can be applied
  {
    insert_internal(nn, ind, key, dd, -1);
  }

  cout << "prefix_sum: " << endl;
  for (int i = 0; i < k2 && prefix_sum[i] != MAX; i++)
  {
    cout << prefix_sum[i] << " ";
  }
  cout << endl;

  cout << "After inserting key " << dd[0] << ": " << endl;
  for (int i = 0; i < k1; i++)
  {
    if (nodes[i] != NULL)
    {
      cout << "nodes[" << i << "]->key: " << endl;
      for (int j = 0; j < nk; j++)
      {
        cout << (nodes[i]->key)[j] << " ";
      }
      cout << endl;
      cout << "nodes[" << i << "]->data: " << endl;
      for (int j = 0; j < nk; j++)
      {
        cout << "data[" << j << "]- " << endl;
        for (int f = 0; f < m; f++)
        {
          cout << (nodes[i]->data)[j][f] << " ";
        }
        cout << endl;
      }
      cout << endl;
    }
  }
}

// Function template to save data to a file
void saveNodeData(const std::string &filename)
{
  std::ofstream ofile;
  ofile.open(filename, std::ios::trunc);
  for (int i = 0; i < node_count; i++)
  {
    if (nodes[i] != NULL)
    {
      for (int j = 0; j < nk; j++)
      {
        ofile << (nodes[i]->key)[j] << endl;
      }
      for (int j = 0; j < nk; j++)
      {
        for (int f = 0; f < m; f++)
        {
          ofile << (nodes[i]->data)[j][f] << endl;
        }
      }
    }
  }
}
bool isFileEmpty(const std::string &filename)
{
  std::ifstream file(filename);
  return file.peek() == std::ifstream::traits_type::eof();
}

// Function template to load data from a file
void loadNodeData(const std::string &filename)
{

  if (isFileEmpty(filename))
    return;
  std::fstream ifile(filename, std::ios_base::in);

  for (int i = 0; i < node_count; i++)
  {
    nodes[i] = (node *)malloc(sizeof(node));
    init(nodes[i]);
    for (int j = 0; j < nk; j++)
    {
      ifile >> (nodes[i]->key)[j];
    }
    for (int j = 0; j < nk; j++)
    {
      for (int f = 0; f < m; f++)
      {
        ifile >> (nodes[i]->data)[j][f];
      }
    }
  }
}

// Function to save a single int value to a file
void saveInt(const std::string &filename)
{
  std::ofstream ofile;
  ofile.open(filename, std::ios::trunc);
  ofile << node_count << endl;
}

// Function to load a single int value from a file
int loadInt(const std::string &filename)
{
  if (isFileEmpty(filename))
    return 0;

  std::fstream myfile(filename, std::ios_base::in);
  int a;
  myfile >> a;
  return a;
}

void loadPrefixArrayData(const std::string &filename)
{
  std::fstream ifile(filename, std::ios_base::in);
  for (int i = 0; i < node_count; i++)
  {
    ifile >> prefix_sum[i];
  }
}

void savePrefixArrayData(const std::string &filename)
{
  std::ofstream ofile;
  ofile.open(filename, std::ios::trunc);
  for (int i = 0; i < node_count; i++)
    ofile << prefix_sum[i] << endl;
}

int main(int argc, char *argv[])
{
  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < k1; i++)
  {
    nodes[i] = NULL;
    if (i < k2)
      prefix_sum[i] = MAX;
  }

  node_count = loadInt("node_count.txt");
  loadNodeData("nodes.txt");
  loadPrefixArrayData("prefix_sum.txt");

  int q = atoi(argv[1]);
  // cout << "Enter number of queries:- " << endl;
  // cin >> q; // no_of_queries

  for (int i = 0; i < q; i++)
  {
    cout << "Enter 1 for inserting a tuple" << endl;
    cout << "Enter 2 for searching for a key" << endl;
    cout << "Enter 3 for doing range_query" << endl;
    int kk = atoi(argv[2]);

    if (kk == 1) // insert
    {
      int dd[m];
      cout << "Enter " << m << " elements of a tuple, of which 1st should be key:- " << endl;
      for (int i = 0; i < m; i++)
      {
        dd[i] = atoi(argv[3 + i]);
        // cout<<"i: "<<i<<endl;
      }
      flag = false;
      insert(dd);
    }
    else if (kk == 2) // search
    {
      cout << "Enter key to be searched" << endl;
      int ke = atoi(argv[3]);
      if (node_count == 0)
      {
        cout << -1 << "\n";
      }
      else
      {
        search(ke);
      }
    }
    else if (kk == 3) // update
    {
      int dd[m];
      cout << "Enter " << m << " elements of a tuple, of which 1st should be key:- " << endl;
      for (int i = 0; i < m; i++)
      {
        dd[i] = atoi(argv[3 + i]);
        // cout<<"i: "<<i<<endl;
      }
      flag = false;
      update(dd);
    }
    else if (kk == 4)
    {
      cout << "Enter key to be searched" << endl;
      int ke = atoi(argv[3]);
      deleteKey(ke);
    }
  }

  saveInt("node_count.txt");
  saveNodeData("nodes.txt"); // Save Node** data
  savePrefixArrayData("prefix_sum.txt");

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  std::cout << "Time taken: " << duration.count() << std::endl;

  return 0;
}
