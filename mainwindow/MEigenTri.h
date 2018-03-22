#ifndef MEITENTRI_H
#define MEIGENTRI_H

#include <vector>

#include <gp_Pnt.hxx>

class MPoint
{
public:
	MPoint(const gp_Pnt &pnt) // from gp_Pnt
	{
		m_x = pnt.X();
		m_y = pnt.Y();
		m_z = pnt.Z();
	}
	MPoint(const MPoint &mp) // from MPoint
	{
		m_x = mp[0];
		m_y = mp[1];
		m_z = mp[2];
	}
	MPoint(double x, double y, double z) :m_x(x), m_y(y), m_z(z) {} // from x, y, z
	MPoint() = delete;

	double operator[] (int index) const
	{
		switch (index)
		{
		case 0: return m_x; break;
		case 1: return m_y; break;
		case 2: return m_z; break;
		default:return 0.0;	break;
		}
	}
	friend MPoint operator- (const MPoint &a, const MPoint &b);
private:
	double m_x;
	double m_y;
	double m_z;
};

MPoint operator- (const MPoint &a, const MPoint &b);

class MEigenTri
{

public:
	MEigenTri() = delete;
	MEigenTri(const std::vector<gp_Pnt> &vec_pnt3d);
	~MEigenTri();

	void perform();
	bool isDone();

	void mean(gp_Pnt &pnt);
	void vectors(gp_Pnt &v_x, gp_Pnt &v_y, gp_Pnt &v_z);

	void matrix(double *m);

private:
	bool calculate();
	void calc_mean();
	void calc_matrix();
	void calc_vectors();

private:
	std::vector<gp_Pnt> m_pnt3d;

	gp_Pnt m_mean;
	gp_Pnt m_vectors[3];

	bool success;

	double m_matrix[3][3];

	std::vector<double> m_area;
};

#endif // !MEITENTRI_H


