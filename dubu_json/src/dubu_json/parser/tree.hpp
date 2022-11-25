#pragma once

#include <cassert>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

namespace shared {

template <class _Alloc, class _Ty>
using _Rebind_alloc_t = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Ty>;

enum class _Tree_child {
  _Right,
  _Left,
};

template <class _Nodeptr>
struct _Tree_id {
  _Nodeptr    _Parent;
  _Tree_child _Child;
};

template <class _Nodeptr>
struct _Tree_find_result {
  _Tree_id<_Nodeptr> _Location;
  _Nodeptr           _Bound;
};

template <class _Mytree>
class _Tree_const_iterator {
public:
  using iterator_category = std::bidirectional_iterator_tag;

  using _Nodeptr        = typename _Mytree::_Nodeptr;
  using value_type      = typename _Mytree::value_type;
  using difference_type = typename _Mytree::difference_type;

  using pointer   = typename _Mytree::const_pointer;
  using reference = const value_type&;

  _Tree_const_iterator() noexcept
      : _Ptr() {}
  _Tree_const_iterator(_Nodeptr _Pnode) noexcept
      : _Ptr(_Pnode) {}

  [[nodiscard]] reference operator*() const noexcept { return _Ptr->_Myval; }

  [[nodiscard]] pointer operator->() const noexcept {
    return std::pointer_traits<pointer>::pointer_to(**this);
  }

  _Tree_const_iterator& operator++() noexcept {
    if (_Ptr->_Right->_Isnil) {
      _Nodeptr _Pnode = _Ptr->_Parent;

      while (!(_Pnode = _Ptr->_Parent)->_Isnil && _Ptr == _Pnode->_Right) {
        _Ptr = _Pnode;
      }

      _Ptr = _Pnode;
    } else {
      _Ptr = _Mytree::_Min(_Ptr->_Right);
    }
    return *this;
  }

  _Tree_const_iterator operator++(int) noexcept {
    _Tree_const_iterator _Tmp = *this;
    ++*this;
    return _Tmp;
  }

  _Tree_const_iterator& operator--() noexcept {
    if (_Ptr->_Isnil) {
      _Ptr = _Ptr->_Right;
    } else if (_Ptr->_Left->_Isnil) {
      _Nodeptr _Pnode = _Ptr->_Parent;

      while (!(_Pnode = _Ptr->_Parent)->_Isnil && _Ptr == _Pnode->_Left) {
        _Ptr = _Pnode;
      }

      if (!_Ptr->_Isnil) {
        _Ptr = _Pnode;
      }
    } else {
      _Ptr = _Mytree::_Max(_Ptr->_Left);
    }
    return *this;
  }

  _Tree_const_iterator operator--(int) noexcept {
    _Tree_const_iterator _Tmp = *this;
    --*this;
    return _Tmp;
  }

  [[nodiscard]] bool operator==(const _Tree_const_iterator& _Right) const noexcept {
    return _Ptr == _Right._Ptr;
  }
  [[nodiscard]] bool operator!=(const _Tree_const_iterator& _Right) const noexcept {
    return !(*this == _Right);
  }

  _Nodeptr _Ptr;
};

template <class _Mytree>
class _Tree_iterator : public _Tree_const_iterator<_Mytree> {
public:
  using _Mybase           = _Tree_const_iterator<_Mytree>;
  using iterator_category = std::bidirectional_iterator_tag;

  using _Nodeptr        = typename _Mytree::_Nodeptr;
  using value_type      = typename _Mytree::value_type;
  using difference_type = typename _Mytree::difference_type;

  using pointer   = typename _Mytree::pointer;
  using reference = value_type&;

  using _Mybase::_Mybase;

  [[nodiscard]] reference operator*() const noexcept {
    return const_cast<reference>(_Mybase::operator*());
  }
  [[nodiscard]] pointer operator->() const noexcept {
    return std::pointer_traits<pointer>::pointer_to(**this);
  }

  _Tree_iterator& operator++() noexcept {
    _Mybase::operator++();
    return *this;
  }
  _Tree_iterator operator++(int) noexcept {
    _Tree_iterator _Tmp = *this;

    _Mybase::operator++();
    return _Tmp;
  }

  _Tree_iterator& operator--() noexcept {
    _Mybase::operator--();
    return *this;
  }
  _Tree_iterator operator--(int) noexcept {
    _Tree_iterator _Tmp = *this;

    _Mybase::operator--();
    return _Tmp;
  }
};

template <class _Ty>
struct _Tree_node {
  using _Nodeptr   = _Tree_node*;
  using value_type = _Ty;
  bool       _Isnil;
  value_type _Myval;
  int        _Height;
  int        _Size;
  int        _Balance;
  _Nodeptr   _Left;
  _Nodeptr   _Parent;
  _Nodeptr   _Right;

  _Tree_node(const _Tree_node&)            = delete;
  _Tree_node& operator=(const _Tree_node&) = delete;

  template <class _Alloc>
  static _Nodeptr _Buyheadnode(_Alloc& _Al) {
    static_assert(std::is_same_v<typename _Alloc::value_type, _Tree_node>, "Bad _Buyheadnode call");

    const auto _Pnode = std::allocator_traits<_Alloc>::allocate(_Al, 1);
    _Pnode->_Left     = _Pnode;
    _Pnode->_Parent   = _Pnode;
    _Pnode->_Right    = _Pnode;
    _Pnode->_Height   = 0;
    _Pnode->_Size     = 0;
    _Pnode->_Balance  = 0;
    _Pnode->_Isnil    = true;
    return _Pnode;
  }

  template <class _Alloc, class... _Valty>
  static _Nodeptr _Buynode(_Alloc& _Al, _Nodeptr _Myhead, _Valty&&... _Val) {
    static_assert(std::is_same_v<typename _Alloc::value_type, _Tree_node>, "Bad _Buynode call");

    const auto _Newnode = std::allocator_traits<_Alloc>::allocate(_Al, 1);
    std::allocator_traits<_Alloc>::construct(
        _Al, std::addressof(_Newnode->_Myval), std::forward<_Valty>(_Val)...);
    _Newnode->_Left    = _Myhead;
    _Newnode->_Parent  = _Myhead;
    _Newnode->_Right   = _Myhead;
    _Newnode->_Height  = 1;
    _Newnode->_Size    = 1;
    _Newnode->_Balance = 0;
    _Newnode->_Isnil   = false;
    return _Newnode;
  }

  template <class _Alloc>
  static void _Freenode0(_Alloc& _Al, _Nodeptr _Ptr) noexcept {
    static_assert(std::is_same_v<typename _Alloc::value_type, _Tree_node>, "Bad _Freenode call");
    _Ptr->_Left   = nullptr;
    _Ptr->_Parent = nullptr;
    _Ptr->_Right  = nullptr;
    std::allocator_traits<_Alloc>::deallocate(_Al, _Ptr, 1);
  }

  template <class _Alloc>
  static void _Freenode(_Alloc& _Al, _Nodeptr _Ptr) noexcept {
    static_assert(std::is_same_v<typename _Alloc::value_type, _Tree_node>, "Bad _Freenode call");
    std::allocator_traits<_Alloc>::destroy(_Al, std::addressof(_Ptr->_Myval));
    _Freenode0(_Al, _Ptr);
  }
};

template <class _Alnode>
struct _Tree_temp_node {
  using _Alnode_traits = std::allocator_traits<_Alnode>;
  using _Nodeptr       = typename _Alnode_traits::pointer;

  template <class... _Valtys>
  explicit _Tree_temp_node(_Alnode& _Al_, _Nodeptr _Myhead, _Valtys&&... _Vals)
      : _Al(_Al_) {
    _Ptr = nullptr;
    _Ptr = _Alnode_traits::allocate(_Al, 1);
    _Alnode_traits::construct(_Al, std::addressof(_Ptr->_Myval), std::forward<_Valtys>(_Vals)...);
    _Ptr->_Left    = _Myhead;
    _Ptr->_Parent  = _Myhead;
    _Ptr->_Right   = _Myhead;
    _Ptr->_Height  = 1;
    _Ptr->_Size    = 1;
    _Ptr->_Balance = 0;
    _Ptr->_Isnil   = false;
  }

  _Tree_temp_node(const _Tree_temp_node&)            = delete;
  _Tree_temp_node& operator=(const _Tree_temp_node&) = delete;

  ~_Tree_temp_node() {
    if (_Ptr) {
      _Alnode_traits::destroy(_Al, std::addressof(_Ptr->_Myval));
      _Alnode_traits::deallocate(_Al, _Ptr, 1);
    }
  }

  _Nodeptr _Release() noexcept { return std::exchange(_Ptr, nullptr); }

  _Alnode& _Al;
  _Nodeptr _Ptr;
};

template <class _Val_types>
class _Tree_val {
public:
  using _Nodeptr = typename _Val_types::_Nodeptr;

  using value_type      = typename _Val_types::value_type;
  using size_type       = typename _Val_types::size_type;
  using difference_type = typename _Val_types::difference_type;
  using pointer         = typename _Val_types::pointer;
  using const_pointer   = typename _Val_types::const_pointer;
  using reference       = value_type&;
  using const_reference = const value_type&;

  using const_iterator = _Tree_const_iterator<_Tree_val>;

public:
  _Tree_val() noexcept
      : _Myhead(nullptr)
      , _Mysize(0) {}

  static _Nodeptr _Max(_Nodeptr _Pnode) noexcept {
    while (!_Pnode->_Right->_Isnil) {
      _Pnode = _Pnode->_Right;
    }
    return _Pnode;
  }
  static _Nodeptr _Min(_Nodeptr _Pnode) noexcept {
    while (!_Pnode->_Left->_Isnil) {
      _Pnode = _Pnode->_Left;
    }
    return _Pnode;
  }

  _Nodeptr _Insert_node(const _Tree_id<_Nodeptr> _Loc, const _Nodeptr _Newnode) {
    ++_Mysize;

    const auto _Head = _Myhead;
    if (_Loc._Parent == _Head) {
      _Head->_Left   = _Newnode;
      _Head->_Parent = _Newnode;
      _Head->_Right  = _Newnode;
      _Update_metadata(_Newnode);
      return _Newnode;
    }

    if (_Loc._Child == _Tree_child::_Right) {
      _Loc._Parent->_Right = _Newnode;
      if (_Loc._Parent == _Head->_Right) {
        _Head->_Right = _Newnode;
      }
    } else {
      _Loc._Parent->_Left = _Newnode;
      if (_Loc._Parent == _Head->_Left) {
        _Head->_Left = _Newnode;
      }
    }
    _Newnode->_Parent = _Loc._Parent;

    _Update_metadata(_Newnode);

    for (_Nodeptr _Pnode = _Newnode; !_Pnode->_Isnil;) {
      if (_Pnode->_Balance >= 2) {
        if (_Pnode->_Right->_Balance < 0) {
          _Rrotate(_Pnode->_Right);
        }
        _Lrotate(_Pnode);
        _Update_metadata(_Pnode);
        break;
      } else if (_Pnode->_Balance <= -2) {
        if (_Pnode->_Left->_Balance > 0) {
          _Lrotate(_Pnode->_Left);
        }
        _Rrotate(_Pnode);
        _Update_metadata(_Pnode);
        break;
      } else {
        _Pnode = _Pnode->_Parent;
      }
    }

    return _Newnode;
  }

  _Nodeptr _Extract(const_iterator _Where) noexcept {
    _Nodeptr _Parent = _Where._Ptr->_Parent;

    _Nodeptr _Erasednode;

    if (_Parent->_Isnil) {
      _Erasednode = _Delete_by_copying(_Parent->_Parent);
    } else if (_Parent->_Left == _Where._Ptr) {
      _Erasednode = _Delete_by_copying(_Parent->_Left);
    } else {
      _Erasednode = _Delete_by_copying(_Parent->_Right);
    }

    {
      _Myhead->_Left  = _Myhead;
      _Myhead->_Right = _Myhead;
      _Myhead->_Left  = _Min(_Myhead->_Parent);
      _Myhead->_Right = _Max(_Myhead->_Parent);
    }

    for (_Nodeptr _Pnode = _Erasednode; !_Pnode->_Isnil;) {
      _Nodeptr _Next = _Pnode->_Parent;
      _Update_metadata_for_node(_Pnode);
      if (_Pnode->_Balance >= 2) {
        if (_Pnode->_Right->_Balance < 0) {
          _Rrotate(_Pnode->_Right);
        }
        _Lrotate(_Pnode);
      } else if (_Pnode->_Balance <= -2) {
        if (_Pnode->_Left->_Balance > 0) {
          _Lrotate(_Pnode->_Left);
        }
        _Rrotate(_Pnode);
      }
      _Pnode = _Next;
    }

    if (_Mysize > 0) {
      --_Mysize;
    }

    return _Where._Ptr;
  }

private:
  _Nodeptr _Delete_by_copying(_Nodeptr& _Pnode) {
    _Nodeptr _Tmp         = _Pnode;
    _Nodeptr _Grandparent = _Pnode->_Parent;

    if (_Pnode->_Right->_Isnil) {
      _Pnode = _Pnode->_Left;
      if (!_Pnode->_Isnil) {
        _Pnode->_Parent = _Grandparent;
      }
    } else if (_Pnode->_Left->_Isnil) {
      _Pnode = _Pnode->_Right;
      if (!_Pnode->_Isnil) {
        _Pnode->_Parent = _Grandparent;
      }
    } else {
      _Nodeptr _P = _Pnode;
      _Nodeptr _Q = _Max(_Pnode->_Left);
      _Nodeptr _T = _Q->_Parent;

      _Pnode      = _Q;
      _Q->_Parent = _Grandparent;

      _Q->_Right          = _P->_Right;
      _Q->_Right->_Parent = _Q;

      _Tmp = _Q;

      if (_T != _P) {
        _Tmp = _T;

        _Nodeptr _R = _Q->_Left;

        _T->_Right = _R;
        if (!_R->_Isnil) {
          _R->_Parent = _T;
          _Tmp        = _R;
        }

        _Q->_Left          = _P->_Left;
        _Q->_Left->_Parent = _Q;
      }
    }

    return _Tmp;
  }

  void _Update_metadata(_Nodeptr _P) {
    while (!(_P = _P->_Parent)->_Isnil) {
      _Update_metadata_for_node(_P);
    }
  }
  void _Update_metadata_for_node(_Nodeptr _P) {
    _P->_Size    = 1 + _P->_Left->_Size + _P->_Right->_Size;
    _P->_Height  = 1 + std::max(_P->_Left->_Height, _P->_Right->_Height);
    _P->_Balance = _P->_Right->_Height - _P->_Left->_Height;
  }

  void _Lrotate(_Nodeptr _Wherenode) {
    _Nodeptr _Pnode    = _Wherenode->_Right;
    _Wherenode->_Right = _Pnode->_Left;

    if (!_Pnode->_Left->_Isnil) {
      _Pnode->_Left->_Parent = _Wherenode;
    }

    _Pnode->_Parent = _Wherenode->_Parent;

    if (_Wherenode == _Myhead->_Parent) {
      _Myhead->_Parent = _Pnode;
    } else if (_Wherenode == _Wherenode->_Parent->_Left) {
      _Wherenode->_Parent->_Left = _Pnode;
    } else {
      _Wherenode->_Parent->_Right = _Pnode;
    }

    _Pnode->_Left       = _Wherenode;
    _Wherenode->_Parent = _Pnode;

    _Update_metadata_for_node(_Wherenode);
    _Update_metadata_for_node(_Pnode);
    if (_Pnode->_Parent != _Myhead) {
      _Update_metadata_for_node(_Pnode->_Parent);
    }
  }

  void _Rrotate(_Nodeptr _Wherenode) {
    _Nodeptr _Pnode   = _Wherenode->_Left;
    _Wherenode->_Left = _Pnode->_Right;

    if (!_Pnode->_Right->_Isnil) {
      _Pnode->_Right->_Parent = _Wherenode;
    }

    _Pnode->_Parent = _Wherenode->_Parent;

    if (_Wherenode == _Myhead->_Parent) {
      _Myhead->_Parent = _Pnode;
    } else if (_Wherenode == _Wherenode->_Parent->_Right) {
      _Wherenode->_Parent->_Right = _Pnode;
    } else {
      _Wherenode->_Parent->_Left = _Pnode;
    }

    _Pnode->_Right      = _Wherenode;
    _Wherenode->_Parent = _Pnode;

    _Update_metadata_for_node(_Wherenode);
    _Update_metadata_for_node(_Pnode);
    if (_Pnode->_Parent != _Myhead) {
      _Update_metadata_for_node(_Pnode->_Parent);
    }
  }

public:
  template <class _Alnode>
  void _Erase_tree(_Alnode& _Al, _Nodeptr _Root) noexcept {
    while (!_Root->_Isnil) {
      _Erase_tree(_Al, _Root->_Right);
      _Alnode::value_type::_Freenode(_Al, std::exchange(_Root, _Root->_Left));
    }
  }
  template <class _Alnode>
  void _Erase_head(_Alnode& _Al) noexcept {
    _Erase_tree(_Al, _Myhead->_Parent);
    _Alnode::value_type::_Freenode0(_Al, _Myhead);
  }

public:
  _Nodeptr  _Myhead;
  size_type _Mysize;
};

template <class _Ty>
struct _Tree_simple_types {
  using value_type      = _Ty;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = value_type*;
  using const_pointer   = const value_type*;
  using _Node           = _Tree_node<_Ty>;
  using _Nodeptr        = _Node*;
};

template <class _Traits>
class _Tree {
public:
  using key_type       = typename _Traits::key_type;
  using value_type     = typename _Traits::value_type;
  using key_compare    = typename _Traits::key_compare;
  using value_compare  = typename _Traits::value_compare;
  using allocator_type = typename _Traits::allocator_type;

  using _Alty        = _Rebind_alloc_t<allocator_type, value_type>;
  using _Alty_traits = std::allocator_traits<_Alty>;

  using _Node          = _Tree_node<value_type>;
  using _Alnode        = _Rebind_alloc_t<allocator_type, _Node>;
  using _Alnode_traits = std::allocator_traits<_Alnode>;
  using _Nodeptr       = typename _Alnode_traits::pointer;

  using _Scary_val = _Tree_val<_Tree_simple_types<value_type>>;

  using size_type       = typename _Alty_traits::size_type;
  using difference_type = typename _Alty_traits::difference_type;
  using pointer         = typename _Alty_traits::pointer;
  using const_pointer   = typename _Alty_traits::const_pointer;
  using reference       = value_type&;
  using const_reference = const value_type&;

  using iterator       = _Tree_iterator<_Scary_val>;
  using const_iterator = _Tree_const_iterator<_Scary_val>;

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
  _Tree(const key_compare& _Parg = key_compare())
      : _Mycomp(_Parg) {
    _Alloc_sentinel();
  }

  _Tree(const key_compare& _Parg, const allocator_type& _Al)
      : _Mycomp(_Parg)
      , _Myallocator(_Al) {
    _Alloc_sentinel();
  }

public:
  _Tree(const _Tree& _Right)
      : _Mycomp(_Right.key_comp())
      , _Myallocator(_Right._Getal()) {
    _Alloc_sentinel();
    _Copy(_Right);
  }

  _Tree& operator=(const _Tree& _Right) {
    if (this != std::addressof(_Right)) {
      _Copy_assign(_Right);
    }
    return *this;
  }

private:
  void _Copy_assign(const _Tree& _Right) {
    clear();
    _Getcomp() = _Right._Getcomp();
    _Getal()   = _Right._Getal();
    _Copy(_Right);
  }
  void _Copy(const _Tree& _Right) {
    const auto _Scary       = _Get_scary();
    const auto _Right_scary = _Right._Get_scary();

    _Scary->_Myhead->_Parent = _Copy_nodes(_Right_scary->_Myhead->_Parent, _Scary->_Myhead);

    _Scary->_Mysize = _Right_scary->_Mysize;
    if (!_Scary->_Myhead->_Parent->_Isnil) {
      _Scary->_Myhead->_Left  = _Scary_val::_Min(_Scary->_Myhead->_Parent);
      _Scary->_Myhead->_Right = _Scary_val::_Max(_Scary->_Myhead->_Parent);
    } else {
      _Scary->_Myhead->_Left  = _Scary->_Myhead;
      _Scary->_Myhead->_Right = _Scary->_Myhead;
    }
  }
  _Nodeptr _Copy_nodes(_Nodeptr _Root, _Nodeptr _Where) {
    const auto _Scary   = _Get_scary();
    _Nodeptr   _Newroot = _Scary->_Myhead;

    if (!_Root->_Isnil) {
      _Nodeptr _Pnode = _Buynode(_Root->_Myval);
      _Pnode->_Parent = _Where;

      _Pnode->_Height  = _Root->_Height;
      _Pnode->_Size    = _Root->_Size;
      _Pnode->_Balance = _Root->_Balance;

      _Newroot = _Pnode;

      _Pnode->_Left  = _Copy_nodes(_Root->_Left, _Pnode);
      _Pnode->_Right = _Copy_nodes(_Root->_Right, _Pnode);
    }

    return _Newroot;
  }

public:
  _Tree(_Tree&& _Right)
      : _Mycomp(_Right.key_comp())
      , _Myallocator(std::move(_Right._Getal())) {
    _Alloc_sentinel();
    _Swap_val(_Right);
  }

  _Tree& operator=(_Tree&& _Right) {
    if (this != std::addressof(_Right)) {
      _Move_assign(_Right);
    }
    return *this;
  }

private:
  void _Swap_val(_Tree& _Right) {
    const auto _Scary       = _Get_scary();
    const auto _Right_scary = _Right._Get_scary();
    std::swap(_Scary->_Myhead, _Right_scary->_Myhead);
    std::swap(_Scary->_Mysize, _Right_scary->_Mysize);
  }
  void _Move_assign(_Tree& _Right) {
    clear();
    _Getcomp() = _Right._Getcomp();
    _Getal()   = std::move(_Right._Getal());
    _Swap_val(_Right);
  }

public:
  ~_Tree() noexcept {
    const auto _Scary = _Get_scary();
    _Scary->_Erase_head(_Getal());
  }

  void clear() noexcept {
    const auto _Scary = _Get_scary();
    auto       _Head  = _Scary->_Myhead;
    _Scary->_Erase_tree(_Getal(), _Head->_Parent);
    _Head->_Left    = _Head;
    _Head->_Parent  = _Head;
    _Head->_Right   = _Head;
    _Scary->_Mysize = 0;
  }

public:
  iterator  erase(iterator _Where) noexcept { return iterator(_Erase(_Where)); }
  iterator  erase(const_iterator _Where) noexcept { return iterator(_Erase(_Where)); }
  size_type erase(const key_type& _Keyval) noexcept {
    const auto _Loc = _Find_lower_bound(_Keyval);
    if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
      _Erase(const_iterator(_Loc._Bound));
      return 1;
    }
    return 0;
  }

private:
  _Nodeptr _Erase(const_iterator _Where) noexcept {
    auto _Successor = _Where;
    ++_Successor;

    _Nodeptr _Erasednode = _Get_scary()->_Extract(_Where);
    _Node::_Freenode(_Getal(), _Erasednode);

    return _Successor._Ptr;
  }

public:
  [[nodiscard]] iterator       begin() noexcept { return iterator(_Get_scary()->_Myhead->_Left); }
  [[nodiscard]] const_iterator begin() const noexcept {
    return const_iterator(_Get_scary()->_Myhead->_Left);
  }
  [[nodiscard]] iterator       end() noexcept { return iterator(_Get_scary()->_Myhead); }
  [[nodiscard]] const_iterator end() const noexcept {
    return const_iterator(_Get_scary()->_Myhead);
  }

  [[nodiscard]] reverse_iterator       rbegin() noexcept { return reverse_iterator(end()); }
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  [[nodiscard]] reverse_iterator       rend() noexcept { return reverse_iterator(begin()); }
  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  [[nodiscard]] const_iterator         cbegin() const noexcept { return begin(); }
  [[nodiscard]] const_iterator         cend() const noexcept { return end(); }
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  [[nodiscard]] const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] size_type size() const noexcept { return _Get_scary()->_Mysize; }
  [[nodiscard]] size_type max_size() const noexcept {
    return std::min(static_cast<size_type>(std::numeric_limits<difference_type>::max()),
                    _Alnode_traits::max_size(_Getal()));
  }
  [[nodiscard]] bool           empty() const noexcept { return _Get_scary()->_Mysize == 0; }
  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return static_cast<allocator_type>(_Getal());
  }
  [[nodiscard]] key_compare   key_comp() const { return _Getcomp(); }
  [[nodiscard]] value_compare value_comp() const { return value_compare(key_comp()); }

public:
  std::pair<iterator, bool> insert(const value_type& _Val) {
    const auto _Result = _Emplace(_Val);
    return {iterator(_Result.first), _Result.second};
  }
  std::pair<iterator, bool> insert(value_type&& _Val) {
    const auto _Result = _Emplace(std::move(_Val));
    return {iterator(_Result.first), _Result.second};
  }
  template <class... _Valtys>
  std::pair<iterator, bool> emplace(_Valtys&&... _Vals) {
    const auto _Result = _Emplace(std::forward<_Valtys>(_Vals)...);
    return {iterator(_Result.first), _Result.second};
  }
  template <class _Iter>
  void insert(_Iter _First, _Iter _Last) {
    assert(_First <= _Last);
    for (; _First != _Last; ++_First) {
      _Emplace(*_First);
    }
  }
  void insert(std::initializer_list<value_type> _Ilist) { insert(_Ilist.begin(), _Ilist.end()); }

protected:
  template <class... _Valtys>
  std::pair<_Nodeptr, bool> _Emplace(_Valtys&&... _Vals) {
    const auto               _Scary = _Get_scary();
    _Tree_temp_node<_Alnode> _Newnode(_Getal(), _Scary->_Myhead, std::forward<_Valtys>(_Vals)...);
    const auto&              _Keyval = _Traits::_Kfn(_Newnode._Ptr->_Myval);
    const auto               _Loc    = _Find_lower_bound(_Keyval);
    if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
      return {_Loc._Bound, false};
    }
    return {_Scary->_Insert_node(_Loc._Location, _Newnode._Release()), true};
  }

  template <class _Keyty>
  _Tree_find_result<_Nodeptr> _Find_lower_bound(const _Keyty& _Keyval) const {
    const auto                  _Scary = _Get_scary();
    _Tree_find_result<_Nodeptr> _Result{{_Scary->_Myhead->_Parent, _Tree_child::_Right},
                                        _Scary->_Myhead};
    _Nodeptr                    _Trynode = _Result._Location._Parent;
    while (!_Trynode->_Isnil) {
      _Result._Location._Parent = _Trynode;
      if (_Getcomp()(_Traits::_Kfn(_Trynode->_Myval), _Keyval)) {
        _Result._Location._Child = _Tree_child::_Right;
        _Trynode                 = _Trynode->_Right;
      } else {
        _Result._Location._Child = _Tree_child::_Left;
        _Result._Bound           = _Trynode;
        _Trynode                 = _Trynode->_Left;
      }
    }
    return _Result;
  }

  template <class _Keyty>
  bool _Lower_bound_duplicate(_Nodeptr _Bound, const _Keyty& _Keyval) const {
    return !_Bound->_Isnil && !_Getcomp()(_Keyval, _Traits::_Kfn(_Bound->_Myval));
  }

public:
  [[nodiscard]] iterator       find(const key_type& _Keyval) { return iterator(_Find(_Keyval)); }
  [[nodiscard]] const_iterator find(const key_type& _Keyval) const {
    return const_iterator(_Find(_Keyval));
  }
  template <class _Other>
  [[nodiscard]] iterator find(const _Other& _Keyval) {
    return iterator(_Find(_Keyval));
  }
  template <class _Other>
  [[nodiscard]] const_iterator find(const _Other& _Keyval) const {
    return const_iterator(_Find(_Keyval));
  }

  [[nodiscard]] iterator lower_bound(const key_type& _Keyval) {
    return iterator(_Find_lower_bound(_Keyval)._Bound);
  }
  [[nodiscard]] const_iterator lower_bound(const key_type& _Keyval) const {
    return const_iterator(_Find_lower_bound(_Keyval)._Bound);
  }
  template <class _Other>
  [[nodiscard]] iterator lower_bound(const _Other& _Keyval) {
    return iterator(_Find_lower_bound(_Keyval)._Bound);
  }
  template <class _Other>
  [[nodiscard]] const_iterator lower_bound(const _Other& _Keyval) const {
    return const_iterator(_Find_lower_bound(_Keyval)._Bound);
  }

private:
  template <class _Other>
  _Nodeptr _Find(const _Other& _Keyval) const {
    const auto _Loc = _Find_lower_bound(_Keyval);
    if (_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
      return _Loc._Bound;
    }
    return _Get_scary()->_Myhead;
  }

public:
  [[nodiscard]] iterator find_by_order(size_type _Order) noexcept {
    return iterator(_Find_by_order(_Order));
  }
  [[nodiscard]] const_iterator find_by_order(size_type _Order) const noexcept {
    return const_iterator(_Find_by_order(_Order));
  }

private:
  _Nodeptr _Find_by_order(size_type _Order) const noexcept {
    const auto _Scary = _Get_scary();
    if (_Order >= _Scary->_Mysize) {
      return _Scary->_Myhead;
    }

    auto k = _Order + 1;

    _Nodeptr _Root = _Scary->_Myhead->_Parent;

    while (_Root != _Scary->_Myhead) {
      const auto q = static_cast<size_type>(_Root->_Left->_Size);
      if (q + 1 == k) {
        break;
      } else if (q + 1 > k) {
        _Root = _Root->_Left;
      } else {
        _Root = _Root->_Right;
        k -= q + 1;
      }
    }

    return _Root;
  }

public:
  [[nodiscard]] size_type order_of_key(const key_type& _Keyval) const noexcept {
    return _Order_of_key(_Keyval);
  }

  template <class _Other>
  [[nodiscard]] size_type order_of_key(const _Other& _Keyval) const noexcept {
    return _Order_of_key(_Keyval);
  }

private:
  template <class _Other>
  size_type _Order_of_key(const _Other& _Keyval) const noexcept {
    const auto _Scary = _Get_scary();

    _Nodeptr  _Root = _Scary->_Myhead->_Parent;
    size_type order = 0;

    while (_Root != _Scary->_Myhead) {
      if (_Getcomp()(_Keyval, _Traits::_Kfn(_Root->_Myval))) {
        _Root = _Root->_Left;
      } else if (_Getcomp()(_Traits::_Kfn(_Root->_Myval), _Keyval)) {
        order += 1 + static_cast<size_type>(_Root->_Left->_Size);
        _Root = _Root->_Right;
      } else {
        order += static_cast<size_type>(_Root->_Left->_Size);
        _Root = _Scary->_Myhead;
      }
    }

    return order;
  }

protected:
  void _Alloc_sentinel() {
    const auto _Scary = _Get_scary();
    _Scary->_Myhead   = _Node::_Buyheadnode(_Getal());
  }

  template <class... _Valty>
  _Nodeptr _Buynode(_Valty&&... _Val) {
    return _Node::_Buynode(_Getal(), _Get_scary()->_Myhead, std::forward<_Valty>(_Val)...);
  }

  key_compare&       _Getcomp() noexcept { return _Mycomp; }
  const key_compare& _Getcomp() const noexcept { return _Mycomp; }

  _Alnode&       _Getal() noexcept { return _Myallocator; }
  const _Alnode& _Getal() const noexcept { return _Myallocator; }

  _Scary_val*       _Get_scary() noexcept { return std::addressof(_Myscary); }
  const _Scary_val* _Get_scary() const noexcept { return std::addressof(_Myscary); }

private:
  key_compare _Mycomp;
  _Alnode     _Myallocator;
  _Scary_val  _Myscary;
};

template <class _Kty, class _Pr, class _Alloc>
struct _Set_traits {
  using key_type       = _Kty;
  using value_type     = _Kty;
  using key_compare    = _Pr;
  using allocator_type = _Alloc;
  using value_compare  = key_compare;

  static const _Kty& _Kfn(const value_type& _Val) { return _Val; }
};

template <class _Kty, class _Ty, class _Pr, class _Alloc>
struct _Map_traits {
  using key_type       = _Kty;
  using value_type     = std::pair<const _Kty, _Ty>;
  using key_compare    = _Pr;
  using allocator_type = _Alloc;

  class value_compare {
  public:
    [[nodiscard]] bool operator()(const value_type& _Left, const value_type& _Right) const {
      return comp(_Left.first, _Right.first);
    }

  private:
    friend _Tree<_Map_traits>;
    value_compare(key_compare _Pred)
        : comp(_Pred) {}
    key_compare comp;
  };

  template <class _Ty1, class _Ty2>
  static const _Kty& _Kfn(const std::pair<_Ty1, _Ty2>& _Val) {
    return _Val.first;
  }
};

template <class _Kty, class _Pr = std::less<_Kty>, class _Alloc = std::allocator<_Kty>>
struct ordered_set : public _Tree<_Set_traits<_Kty, _Pr, _Alloc>> {
  using _Mybase = _Tree<_Set_traits<_Kty, _Pr, _Alloc>>;

  using key_compare    = _Pr;
  using value_type     = typename _Mybase::value_type;
  using allocator_type = typename _Mybase::allocator_type;

  ordered_set()
      : _Mybase(key_compare()) {}

  explicit ordered_set(const allocator_type& _Al)
      : _Mybase(key_compare(), _Al) {}

  ordered_set(const ordered_set& _Right)
      : _Mybase(_Right) {}
  ordered_set& operator=(const ordered_set& _Right) {
    _Mybase::operator=(_Right);
    return *this;
  }

  ordered_set(ordered_set&& _Right)
      : _Mybase(std::move(_Right)) {}
  ordered_set& operator=(ordered_set&& _Right) {
    _Mybase::operator=(std::move(_Right));
    return *this;
  }

  template <class _Iter>
  ordered_set(_Iter _First, _Iter _Last)
      : _Mybase(key_compare()) {
    this->insert(_First, _Last);
  }
  ordered_set(std::initializer_list<value_type> _Ilist)
      : _Mybase(key_compare()) {
    this->insert(_Ilist);
  }
};

template <class _Kty,
          class _Ty,
          class _Pr    = std::less<_Kty>,
          class _Alloc = std::allocator<std::pair<const _Kty, _Ty>>>
struct ordered_map : public _Tree<_Map_traits<_Kty, _Ty, _Pr, _Alloc>> {
  using _Mybase  = _Tree<_Map_traits<_Kty, _Ty, _Pr, _Alloc>>;
  using _Nodeptr = typename _Mybase::_Nodeptr;

  using key_compare    = _Pr;
  using key_type       = _Kty;
  using mapped_type    = _Ty;
  using value_type     = typename _Mybase::value_type;
  using allocator_type = typename _Mybase::allocator_type;

  using _Alnode = typename _Mybase::_Alnode;

  ordered_map()
      : _Mybase(key_compare()) {}

  explicit ordered_map(const allocator_type& _Al)
      : _Mybase(key_compare(), _Al) {}

  ordered_map(const ordered_map& _Right)
      : _Mybase(_Right) {}
  ordered_map& operator=(const ordered_map& _Right) {
    _Mybase::operator=(_Right);
    return *this;
  }

  ordered_map(ordered_map&& _Right)
      : _Mybase(std::move(_Right)) {}
  ordered_map& operator=(ordered_map&& _Right) {
    _Mybase::operator=(std::move(_Right));
    return *this;
  }

  template <class _Iter>
  ordered_map(_Iter _First, _Iter _Last)
      : _Mybase(key_compare()) {
    this->insert(_First, _Last);
  }

  ordered_map(std::initializer_list<value_type> _Ilist)
      : _Mybase(key_compare()) {
    this->insert(_Ilist);
  }

  mapped_type& operator[](const key_type& _Keyval) {
    return _Try_emplace(_Keyval).first->_Myval.second;
  }
  mapped_type& operator[](key_type&& _Keyval) {
    return _Try_emplace(std::move(_Keyval)).first->_Myval.second;
  }

  [[nodiscard]] mapped_type& at(const key_type& _Keyval) {
    const auto _Loc = _Mybase::_Find_lower_bound(_Keyval);
    if (!_Mybase::_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
      throw std::runtime_error("Invalid ordered_set<K,V> key");
    }
    return _Loc._Bound->_Myval.second;
  }
  [[nodiscard]] const mapped_type& at(const key_type& _Keyval) const {
    const auto _Loc = _Mybase::_Find_lower_bound(_Keyval);
    if (!_Mybase::_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
      throw std::runtime_error("Invalid ordered_set<K,V> key");
    }
    return _Loc._Bound->_Myval.second;
  }

private:
  template <class _Keyty, class... _Mappedty>
  std::pair<_Nodeptr, bool> _Try_emplace(_Keyty&& _Keyval, _Mappedty&&... _Mapval) {
    const auto _Loc = _Mybase::_Find_lower_bound(_Keyval);
    if (_Mybase::_Lower_bound_duplicate(_Loc._Bound, _Keyval)) {
      return {_Loc._Bound, false};
    }

    const auto _Scary = _Mybase::_Get_scary();
    const auto _Inserted =
        _Tree_temp_node<_Alnode>(_Mybase::_Getal(),
                                 _Scary->_Myhead,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(std::forward<_Keyty>(_Keyval)),
                                 std::forward_as_tuple(std::forward<_Mappedty>(_Mapval)...))
            ._Release();

    return {_Scary->_Insert_node(_Loc._Location, _Inserted), true};
  }
};

namespace pmr {
template <class _Kty, class _Pr = std::less<_Kty>>
using ordered_set = shared::ordered_set<_Kty, _Pr, std::pmr::polymorphic_allocator<_Kty>>;

template <class _Kty, class _Ty, class _Pr = std::less<_Kty>>
using ordered_map = shared::
    ordered_map<_Kty, _Ty, _Pr, std::pmr::polymorphic_allocator<std::pair<const _Kty, _Ty>>>;
}  // namespace pmr

}  // namespace shared