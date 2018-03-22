#include "MEigenTri.h"

// libaray Eigen
#include <Eigen\EigenValues>

MPoint operator- (const MPoint &a, const MPoint &b)
{
	return MPoint(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

MEigenTri::MEigenTri(const std::vector<gp_Pnt> &vec_pnt3d)
	: success(false)
{
	m_area.clear();
	m_pnt3d.clear();

	if (vec_pnt3d.size() > 1)
	{
		m_pnt3d.insert(m_pnt3d.begin(), vec_pnt3d.begin(), vec_pnt3d.end());
	}
}

MEigenTri::~MEigenTri()
{
}

void MEigenTri::perform()
{
	if (m_pnt3d.size() > 1)
	{
		success = calculate();
	}
}

bool MEigenTri::isDone()
{
	return success;
}

void MEigenTri::mean(gp_Pnt &pnt)
{
	pnt = m_mean;
}

void MEigenTri::vectors(gp_Pnt &v_x, gp_Pnt &v_y, gp_Pnt &v_z)
{
	v_x = m_vectors[0];
	v_y = m_vectors[1];
	v_z = m_vectors[2];
}

void MEigenTri::matrix(double *m)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m[i * 3 + j] = m_matrix[i][j];
		}
	}
}

// private
bool MEigenTri::calculate()
{
	try
	{
		calc_mean();
		calc_matrix();
		calc_vectors();
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void MEigenTri::calc_mean()
{
#ifdef M_TRIANGULAR
	auto L_F_area = [](const gp_Pnt &p, const gp_Pnt &q, const gp_Pnt &r) -> double
	{
		double dis_1 = p.Distance(q);
		double dis_2 = p.Distance(r);
		return (dis_1 * dis_2) / 2.0;
	};

	size_t size = m_pnt3d.size();	
	gp_Pnt mu(0.0, 0.0, 0.0);
	for (size_t i = 0; i < size; i += 3)
	{
		m_area.push_back(L_F_area(m_pnt3d[i], m_pnt3d[i + 1], m_pnt3d[i + 2]));

		double X = m_pnt3d[i].X() + m_pnt3d[i + 1].X() + m_pnt3d[i + 2].X();
		double Y = m_pnt3d[i].Y() + m_pnt3d[i + 1].Y() + m_pnt3d[i + 2].Y();
		double Z = m_pnt3d[i].Z() + m_pnt3d[i + 1].Z() + m_pnt3d[i + 2].Z();

		mu.SetX(X / m_area.back() + mu.X());
		mu.SetY(Y / m_area.back() + mu.Y());
		mu.SetZ(Z / m_area.back() + mu.Z());
	}

	size = m_area.size();
	m_mean = gp_Pnt(
		mu.X() / double(size * 6),
		mu.Y() / double(size * 6),
		mu.Z() / double(size * 6));

#else

	double mu_x = 0.0;
	double mu_y = 0.0;
	double mu_z = 0.0;

	size_t size = m_pnt3d.size();
	for (size_t i = 0; i < size; i++)
	{
		mu_x += m_pnt3d[i].X();
		mu_y += m_pnt3d[i].Y();
		mu_z += m_pnt3d[i].Z();
	}
	m_mean.SetX(mu_x / (double)size);
	m_mean.SetY(mu_y / (double)size);
	m_mean.SetZ(mu_z / (double)size);

#endif // M_TRIANGULAR
}

void MEigenTri::calc_matrix()
{
#ifdef M_TRIANGULAR

	size_t size = m_pnt3d.size() / 3;

	MPoint mu(m_mean);

	double element = 0.0;
	for (int j = 0; j < 3; j++) // row
	{
		for (int k = 0; k < 3; k++) // column
		{
			element = 0.0; // element(j, k) in matrix

			for (size_t i = 0; i < size; i ++)
			{
				double tmp = 0.0;
				// delta between <p, q, r> and <mu>
				MPoint d_p(MPoint(m_pnt3d[i * 3]) - mu);
				MPoint d_q(MPoint(m_pnt3d[i * 3 + 1]) - mu);
				MPoint d_r(MPoint(m_pnt3d[i * 3 + 2]) - mu);

				tmp = (d_p[j] + d_q[j] + d_r[j]) * (d_p[k] + d_q[k] + d_r[k]) +
					d_p[j] * d_p[k] + d_q[j] * d_q[k] + d_r[j] * d_r[k];
				element += tmp * m_area[i];
			}
			element /= static_cast<double>(m_area.size());
			m_matrix[j][k] = element;
		} // row
	} // column

#else

	MPoint mu(m_mean);

	size_t size = m_pnt3d.size();
	for (int m = 0; m < 3; m++) // row
	{
		for (int n = 0; n < 3; n++) // column
		{
			double element = 0.0; // Element(m,n) in matrix C
			for (size_t i = 0; i < size; i++) // points
			{
				MPoint mpnt(m_pnt3d[i].X(), m_pnt3d[i].Y(), m_pnt3d[i].Z());

				element += (mpnt[m] - mu[m]) * (mpnt[n] - mu[n]);
			}
			element /= size;

			m_matrix[m][n] = element;
		} // loop-column
	} // loop-row

#endif // M_TRIANGULAR
}

void MEigenTri::calc_vectors()
{
	Eigen::Matrix3d mtx;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mtx(i, j) = m_matrix[i][j];
		}
	}
	Eigen::EigenSolver<Eigen::Matrix3d> svr(mtx, true/*values & vectors*/);

	Eigen::Matrix3cd vectors = svr.eigenvectors();
	Eigen::Vector3cd vector_column;
	for (int i = 0; i < 3; i++)
	{
		vector_column = vectors.col(i);

		m_vectors[i].SetX(vector_column[0].real());
		m_vectors[i].SetY(vector_column[1].real());
		m_vectors[i].SetZ(vector_column[2].real());
	}
}