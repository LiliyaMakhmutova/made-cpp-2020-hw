#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

class Line;

template <typename T>
constexpr T pi = T(3.141592653589793L);

template <typename T>
constexpr T epsilon = T(1e-6);

bool are_equal(double a, double b) {
  double diff = a - b;
  return (diff < epsilon<double>)&&(diff > -epsilon<double>);
}

template <typename N>
inline N deg2rad(N d) {
  return pi<N> * d / 180.0;
}

template <typename N>
inline N rad2deg(N r) {
  return 180.0 * r / pi<N>;
}

struct Point {
  double x;
  double y;

  Point(double x, double y) : x(x), y(y) {}

  Point() : x(0.0), y(0.0) {}

  Point(const Point& p) : x(p.x), y(p.y) {}

  bool operator==(const Point& rhs) {
    return are_equal(this->x, rhs.x) && are_equal(this->y, rhs.y);
  }

  bool operator!=(const Point& rhs) { return !(*this == rhs); }

  bool operator<(const Point& rhs) {
    if (this->x < rhs.x) {
      return true;
    } else if (are_equal(this->x, rhs.x) && this->y < rhs.y) {
      return true;
    } else {
      return false;
    }
  }

  void rotate(Point center, double angle) {
    double cos_ = cos(deg2rad<double>(angle));
    double sin_ = sin(deg2rad<double>(angle));

    double new_x = (cos_ * (x - center.x) - sin_ * (y - center.y) + center.x);
    double new_y = (sin_ * (x - center.x) + cos_ * (y - center.y) + center.y);
    this->x = new_x;
    this->y = new_y;
  }

  Point rotate(Point center, double angle) const {
    Point p(*this);
    p.rotate(center, angle);
    return p;
  }

  void reflex(Point center) {
    double new_x = 2 * center.x - this->x;
    double new_y = 2 * center.y - this->y;
    this->x = new_x;
    this->y = new_y;
  }

  void scale(Point center, double coefficient) {
    double a = center.x;
    double b = center.y;
    double new_x = coefficient * (x - a) + a;
    double new_y = coefficient * (y - b) + b;
    this->x = new_x;
    this->y = new_y;
  }

  double dist(const Point& rhs) {
    return sqrt((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
  }
};

class Line {
  friend class Point;

 public:
  explicit Line(Point p1, Point p2)
      : A(p2.y - p1.y), B(p1.x - p2.x), C(p2.x * p1.y - p1.x * p2.y) {}

  explicit Line(double k, double b) : A(k), B(-1), C(b) {}

  explicit Line(Point p, double k) : A(k), B(-1), C(-k * p.x + p.y) {}

  bool operator==(const Line& rhs) const {
    return are_equal(A, rhs.A) && are_equal(B, rhs.B) && are_equal(C, rhs.C);
  }

  bool operator!=(const Line& rhs) const { return !(*this == rhs); }

  void reflex(Point& p) const {
    double x = p.x;
    double y = p.y;
    double t = -1 * (A * x + B * y + C) / (A * A + B * B);
    double x_c = A * t + x;
    double y_c = B * t + y;

    Point center(x_c, y_c);

    p.reflex(center);
  }

  Line() = delete;

  double getA() const { return A; }
  double getB() const { return B; }
  double getC() const { return C; }

 private:
  double A;
  double B;
  double C;
};

class Shape {
 public:
  virtual double perimeter() = 0;
  virtual double area() = 0;
  virtual bool operator==(const Shape& another) = 0;
  virtual void rotate(Point center, double angle) = 0;
  virtual void reflex(Point center) = 0;
  virtual void reflex(Line axis) = 0;
  virtual void scale(Point center, double coefficient) = 0;
};

class Polygon : public Shape {
 public:
  explicit Polygon(const std::vector<Point>& vertices) : vertices(vertices) {}
  size_t verticesCount() { return vertices.size(); }
  std::vector<Point> getVertices() { return vertices; }
  Polygon() = delete;

  Point center(size_t num_sides) {
    double xc, yc;
    xc = yc = 0;
    for (int i = 0; i < num_sides; i++) {
      xc += vertices[i].x;
      yc += vertices[i].y;
    }
    return Point(xc / num_sides, yc / num_sides);
  }

  double perimeter() override {
    double per = 0.0;
    size_t sz = vertices.size();
    for (int i = 1; i < sz; i++) {
      per += vertices[i].dist(vertices[i - 1]);
    }
    per += vertices[0].dist(vertices[sz - 1]);
    return per;
  }

  void rotate(Point center, double angle) override {
    for (auto& iter : vertices) {
      iter.rotate(center, angle);
    }
  }

  void reflex(Point center) override {
    for (auto& iter : vertices) {
      iter.reflex(center);
    }
  }

  void reflex(Line axis) override {
    for (auto& iter : vertices) {
      axis.reflex(iter);
    }
  }

  void scale(Point center, double coefficient) override {
    for (auto& iter : vertices) {
      iter.scale(center, coefficient);
    }
  }

  double area() override {
    double area_times_2 = 0.0;
    size_t sz = vertices.size();
    // count using determinants
    for (int i = 1; i < sz; i++) {
      area_times_2 +=
          vertices[i - 1].x * vertices[i].y - vertices[i - 1].y * vertices[i].x;
    }
    area_times_2 +=
        vertices[sz - 1].x * vertices[0].y - vertices[sz - 1].y * vertices[0].x;
    return area_times_2 / 2;
  }

  bool operator==(const Shape& another) override {
    const Polygon* pol = dynamic_cast<const Polygon*>(&another);
    std::vector<Point> vertices_copy(vertices);
    std::vector<Point> another_vertices_copy(pol->vertices);
    std::sort(vertices_copy.begin(), vertices_copy.end());
    std::sort(another_vertices_copy.begin(), another_vertices_copy.end());
    size_t n = vertices_copy.size();
    size_t m = another_vertices_copy.size();
    if (n != m) {
      return false;
    } else {
      for (int i = 0; i < n; i++) {
        if (vertices_copy[i] != another_vertices_copy[i]) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator!=(const Shape& another) { return !operator==(another); }

 protected:
  std::vector<Point> vertices;

 private:
};

class Ellipse : public Shape {
 public:
  explicit Ellipse(Point f1, Point f2, double dist)
      : f1(f1), f2(f2), dist(dist) {}

  std::pair<Point, Point> focuses() { return {f1, f2}; }

  Ellipse() = delete;

  double eccentricity() {
    std::tuple<double, double, double> t = getABC();
    return std::get<2>(t) / std::get<0>(t);
  }

  Point center() { return Point((f1.x + f2.x) / 2, (f1.y + f2.y) / 2); }

  double area() override {
    std::tuple<double, double, double> t = getABC();
    return std::get<0>(t) * std::get<1>(t) * pi<double>;
  }

  double perimeter() override {
    std::tuple<double, double, double> t = getABC();
    double a = std::get<0>(t);
    double ecc = eccentricity();
    return 4 * a * std::comp_ellint_2(ecc);
  }

  bool operator==(const Shape& another) override {
    const Ellipse* el = dynamic_cast<const Ellipse*>(&another);
    return ((this->f1 == el->f1 && this->f2 == el->f2) ||
            (this->f1 == el->f2 && this->f2 == el->f1)) &&
           are_equal(this->dist, el->dist);
  }

  void rotate(Point center, double angle) override {
    f1.rotate(center, angle);
    f2.rotate(center, angle);
  }

  void reflex(Point center) override {
    f1.reflex(center);
    f2.reflex(center);
  }

  void reflex(Line axis) override {
    axis.reflex(f1);
    axis.reflex(f2);
  }

  void scale(Point center, double coefficient) override {
    f1.scale(center, coefficient);
    f2.scale(center, coefficient);
    dist *= coefficient;
  }

 protected:
  Point f1;
  Point f2;
  double dist;

 private:
  std::tuple<double, double, double> getABC() {
    double first, second, a, b;
    first = dist / 2;
    double c = f1.dist(f2) / 2;
    second = sqrt(first * first - c * c);
    a = std::max(first, second);
    b = std::min(first, second);
    return std::make_tuple(a, b, c);
  }
};

class Circle : public Ellipse {
 public:
  explicit Circle(Point c, double radius) : Ellipse(c, c, radius * 2) {}

  Circle() = delete;

  double radius() { return dist / 2; }
};

class Rectangle : public Polygon {
 public:
  explicit Rectangle(Point p1, Point p2, double num)
      : Polygon({p1, ((const Point)p2).rotate(p1, atan(num)), p2,
                 ((const Point)p1).rotate(p2, atan(1 / num))}) {}

  Rectangle() = delete;

  Point center() { return Polygon::center(4); }

  std::pair<Line, Line> diagonals() {
    return {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
  }
};

class Square : public Rectangle {
 public:
  explicit Square(Point p1, Point p2) : Rectangle(p1, p2, 1.0) {}

  Circle circumscribedCircle() {
    Point center = Rectangle::center();
    double radius =
        sqrt(vertices[0].x * vertices[3].x + vertices[0].y * vertices[3].y);
    return Circle(center, radius);
  }

  Circle inscribedCircle() {
    Point center = Rectangle::center();
    double radius =
        sqrt(vertices[0].x * vertices[1].x + vertices[0].y * vertices[1].y);
    return Circle(center, radius);
  }

  Square() = delete;
};

class Triangle : public Polygon {
 public:
  explicit Triangle(Point p1, Point p2, Point p3) : Polygon({p1, p2, p3}) {}

  Triangle() = delete;

  Point centroid() { return Polygon::center(3); }

  std::pair<Line, Line> diagonals() {
    return {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
  }

  Point orthocenter() {
    Point p1, p2, p3;
    if (vertices[0].y != vertices[1].y && vertices[0].y != vertices[2].y) {
      p1 = vertices[0];
      p2 = vertices[1];
      p3 = vertices[2];
    } else if (vertices[1].y != vertices[2].y &&
               vertices[1].y != vertices[0].y) {
      p1 = vertices[1];
      p2 = vertices[2];
      p3 = vertices[0];
    } else {
      p1 = vertices[0];
      p2 = vertices[2];
      p3 = vertices[1];
    }
    double p_slope_1 = -(p2.x - p1.x) / (p2.y - p1.y);
    double p_slope_2 = -(p3.x - p1.x) / (p3.y - p1.y);
    double x = (p3.y + p_slope_1 * p3.x - p2.y - p_slope_2 * p2.x) /
               (p_slope_1 + p_slope_2);
    double y = (p3.y + p_slope_1 * p3.x) - (p_slope_1 * x);

    return Point(x, y);
  }

  Circle circumscribedCircle() {
    Point A = vertices[0];
    Point B = vertices[1];
    Point C = vertices[2];
    double a = B.dist(C);
    double b = A.dist(C);
    double c = A.dist(B);

    double x =
        ((A.x * A.x + A.y * A.y) * (B.y - C.y) +
         (B.x * B.x + B.y * B.y) * (C.y - A.y) +
         (C.x * C.x + C.y * C.y) * (A.y - B.y)) /
        (2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)));

    double y =
        ((A.x * A.x + A.y * A.y) * (C.x - B.x) +
         (B.x * B.x + B.y * B.y) * (A.x - C.x) +
         (C.x * C.x + C.y * C.y) * (B.x - A.x)) /
        (2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)));

    Point center = Point(x, y);
    double s = (a + b + c) / 2;
    double radius = a * b * c / (4 * sqrt(s * (s - a) * (s - b) * (s - c)));
    return Circle(center, radius);
  }

  Circle inscribedCircle() {
    Point A = vertices[0];
    Point B = vertices[1];
    Point C = vertices[2];
    double a = B.dist(C);
    double b = A.dist(C);
    double c = A.dist(B);

    double x = a * A.x + b * B.x + c * C.x;
    x /= a + b + c;

    double y = a * A.y + b * B.y + c * C.y;
    y /= a + b + c;
    Point center(x, y);

    double s = (a + b + c) / 2;
    double r = sqrt((s - a) * (s - b) * (s - c) / s);
    return Circle(center, r);
  }

  Circle ninePointsCircle() {
    Circle circumscribedCircle_ = circumscribedCircle();
    Point circumcenter = circumscribedCircle_.focuses().first;
    Point orthocenter_ = orthocenter();
    double x = (circumcenter.x + orthocenter_.x) / 2;
    double y = (circumcenter.y + orthocenter_.y) / 2;
    double r = circumscribedCircle_.radius() / 2;

    return Circle(Point(x, y), r);
  }

  Line EulerLine() { return Line(ninePointsCircle().center(), centroid()); }
};
