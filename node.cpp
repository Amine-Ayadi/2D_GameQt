#include "node.h"
Node::Node(int x, int y, float w, Node *ptr) :
w{w}, cost{0}, x{x}, y{y},parent{ptr},
 flag{0}
{
}

Node::Node(int x, int y, float w, float cost, char flag, Node *ptr)
    :
w{w}, cost{cost}, x{x}, y{y},parent{ptr},
 flag{flag}
{

}

Node::Node( const Node & other): w{other.w}, cost{other.cost}, x{other.x}, y{other.y},parent{other.parent},
 flag{other.flag}
{
    //std::cout << "Copy ructor is called" << std::endl;
}
Node::Node(Node && other) noexcept
{
    //std::cout << "Move ructor is called" << std::endl;
    x = other.x;
    y = other.y;
    w = other.w;
    parent = other.parent;
    flag = other.flag;
    cost = other.cost;
}
int Node::getX() const
{
    return x;
}
int Node::getY() const
{
    return y;
}
float Node::getW() const{
    return w;
}

void Node::setW(float nw)
{
    w = nw;
}

float Node::getCost() const
{
    return cost;
}
void Node::setCost(float nw)
{
    cost = nw;
}

void Node::setParent(Node *ptr)
{
    parent = ptr;
}
Node *Node::getParent()
{
    return parent;
}

void Node::setFlag(char f)
{
   flag = f;
}

char Node::getFlag()
{
    return flag;
}

float Node::getHeuristic() const
{
    return heuristic;
}

void Node::setHeuristic(float nh)
{
    heuristic = nh;
}

void Node::setLength(int l)
{
    length = l;
}

int Node::getLength()
{
    return length;
}

void Node::resetSpecial()
{
    flag = 3;
    cost = 0;
    heuristic=0;
    length=0;
    parent=nullptr;

}

void Node::resetSpecialDone()
{
    flag = 4;
    cost = 0;
    heuristic=0;
    length=0;
    parent=nullptr;
}

void Node::resetTile()
{
    flag = 0 ;
    heuristic=0;
    cost = 0;
    length=0;
    parent=nullptr;
}

