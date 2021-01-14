template <typename ExtraT> struct SplayT {
  using Extra = ExtraT;

  uint8_t type;
  uint32_t size;
  SplayT *parent, *children[2];
  Extra data;

  SplayT() : type(2), size(0) {}

  SplayT(Extra data) : type(2), size(1), data(data) {
    parent = children[0] = children[1] = null_node();
  }

  SplayT *update() {
    size = children[0]->size + 1 + children[1]->size;
    return this;
  }

  void rotate() {
    SplayT *x = this;
    uint8_t t = x->type;
    SplayT *y = x->parent, *z = x->children[t ^ 1];
    x->type = y->type, x->parent = y->parent;
    if (x->type != 2) {
      x->parent->children[x->type] = x;
    }
    y->type = t ^ 1, y->parent = x, x->children[t ^ 1] = y;
    if (z != null_node()) {
      z->type = t, z->parent = y;
    }
    y->children[t] = z;
    y->update();
  }

  void splay() {
    SplayT *x = this;
    while (x->type != 2) {
      SplayT *y = x->parent;
      if (x->type == y->type) {
        y->rotate();
      } else {
        x->rotate();
      }
      if (x->type == 2) {
        break;
      }
      x->rotate();
    }
    x->update();
  }

  static SplayT *null_node() {
    static SplayT null_node;
    return &null_node;
  }
};