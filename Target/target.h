#pragma once


class Target
{
public:
    Target(double x, double y, double z);

    void update(double deltaTime);

    double GetX() const {return x;}
    double GetY() const {return y;}
    double GetZ() const {return z;}

    void SetX(double coord_x) {x = coord_x;}
    void SetY(double coord_y) {y = coord_y;}
    void SetZ(double coord_z) {z = coord_z;}
    void SetXYZ(double coord_x, double coord_y, double coord_z);

    double GetVx() const {return vx;}
    double GetVy() const {return vy;}
    double GetVz() const {return vz;}
private:
    double x;
    double y;
    double z;

    double vx;
    double vy;
    double vz;
};

