#include "cli.hpp"
#include "kernel.hpp"
#include "kerr_schild_kerr_test_data.hpp"
#include "log.hpp"
#include "metric_server.hpp"
#include "test_constants.hpp"

#include <gtest/gtest.h>
#include <random>

class Kerr_Schild_Kerr_Tests : public testing::Test {
protected:
  static void SetUpTestSuite() {
    using namespace grlensing;
    using namespace grlensing_tests;
    using std::mt19937_64;
    using std::uniform_real_distribution;

    // Avoid reallocating static objects if called in subclasses of FooTest.
    if (shared_kernel == nullptr) {
      shared_kernel = new kernel();

      // Load global configuration file
      const auto config_file = YAML::LoadFile("./grlensing_testing_config.yaml");

      // Load plugins
      load_plugins(*shared_kernel, config_file);
    }

    if (shared_random_engine == nullptr) {
      shared_random_engine = new mt19937_64(random_seed);
    }

    if (shared_coord_distrib == nullptr) {
      shared_coord_distrib = new uniform_real_distribution<double>(coord_range[0], coord_range[1]);
    }
  }

  static void TearDownTestSuite() {
    delete shared_kernel;
    delete shared_random_engine;
    delete shared_coord_distrib;

    shared_kernel = nullptr;
    shared_random_engine = nullptr;
    shared_coord_distrib = nullptr;
  }

  static grlensing::kernel *shared_kernel;
  static std::mt19937_64 *shared_random_engine;
  static std::uniform_real_distribution<double> *shared_coord_distrib;
};

grlensing::kernel *Kerr_Schild_Kerr_Tests::shared_kernel = nullptr;
std::mt19937_64 *Kerr_Schild_Kerr_Tests::shared_random_engine = nullptr;
std::uniform_real_distribution<double> *Kerr_Schild_Kerr_Tests::shared_coord_distrib = nullptr;

TEST_F(Kerr_Schild_Kerr_Tests, lapse_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Reference metric
  const auto ref_uu_g = uu_g(M, a, x, y, z);

  EXPECT_NEAR(metric->lapse(0.0, x, y, z), sqrt(-1.0 / ref_uu_g[0][0]), double_comp_tol);
}

TEST_F(Kerr_Schild_Kerr_Tests, lower_shift_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Reference metric
  const auto ref_ll_g = ll_g(M, a, x, y, z);

  EXPECT_NEAR(metric->l_shift(0.0, x, y, z)[0], ref_ll_g[0][1], double_comp_tol);
  EXPECT_NEAR(metric->l_shift(0.0, x, y, z)[1], ref_ll_g[0][2], double_comp_tol);
  EXPECT_NEAR(metric->l_shift(0.0, x, y, z)[2], ref_ll_g[0][3], double_comp_tol);
}

TEST_F(Kerr_Schild_Kerr_Tests, upper_shift_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Reference metric
  const auto ref_uu_g = uu_g(M, a, x, y, z);

  EXPECT_NEAR(metric->u_shift(0.0, x, y, z)[0], -ref_uu_g[0][1] / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->u_shift(0.0, x, y, z)[1], -ref_uu_g[0][2] / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->u_shift(0.0, x, y, z)[2], -ref_uu_g[0][3] / ref_uu_g[0][0], double_comp_tol);
}

TEST_F(Kerr_Schild_Kerr_Tests, lower_spatial_metric_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Reference metric
  const auto ref_ll_g = ll_g(M, a, x, y, z);

  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[0][0], ref_ll_g[1][1], double_comp_tol);
  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[0][1], ref_ll_g[1][2], double_comp_tol);
  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[0][2], ref_ll_g[1][3], double_comp_tol);

  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[1][0], ref_ll_g[2][1], double_comp_tol);
  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[1][1], ref_ll_g[2][2], double_comp_tol);
  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[1][2], ref_ll_g[2][3], double_comp_tol);

  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[2][0], ref_ll_g[3][1], double_comp_tol);
  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[2][1], ref_ll_g[3][2], double_comp_tol);
  EXPECT_NEAR(metric->ll_smetric(0.0, x, y, z)[2][2], ref_ll_g[3][3], double_comp_tol);
}

TEST_F(Kerr_Schild_Kerr_Tests, upper_spatial_metric_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Reference metric
  const auto ref_uu_g = uu_g(M, a, x, y, z);

  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[0][0],
              ref_uu_g[1][1] - (ref_uu_g[0][1] * ref_uu_g[0][1]) / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[0][1],
              ref_uu_g[1][2] - (ref_uu_g[0][1] * ref_uu_g[0][2]) / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[0][2],
              ref_uu_g[1][3] - (ref_uu_g[0][1] * ref_uu_g[0][3]) / ref_uu_g[0][0], double_comp_tol);

  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[1][0],
              ref_uu_g[2][1] - (ref_uu_g[0][2] * ref_uu_g[0][1]) / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[1][1],
              ref_uu_g[2][2] - (ref_uu_g[0][2] * ref_uu_g[0][2]) / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[1][2],
              ref_uu_g[2][3] - (ref_uu_g[0][2] * ref_uu_g[0][3]) / ref_uu_g[0][0], double_comp_tol);

  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[2][0],
              ref_uu_g[3][1] - (ref_uu_g[0][3] * ref_uu_g[0][1]) / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[2][1],
              ref_uu_g[3][2] - (ref_uu_g[0][3] * ref_uu_g[0][2]) / ref_uu_g[0][0], double_comp_tol);
  EXPECT_NEAR(metric->uu_smetric(0.0, x, y, z)[2][2],
              ref_uu_g[3][3] - (ref_uu_g[0][3] * ref_uu_g[0][3]) / ref_uu_g[0][0], double_comp_tol);
}

TEST_F(Kerr_Schild_Kerr_Tests, grad_lapse_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Lapse from reference metric
  const auto ref_lapse = [=](double, double x, double y, double z) -> double {
    const auto ref_uu_g = uu_g(M, a, x, y, z);
    return sqrt(-1.0 / ref_uu_g[0][0]);
  };

  const auto d_ref_lapse_dx = fd_4<fd_direction::x, double>(ref_lapse, 0.0, x, y, z);
  const auto d_ref_lapse_dy = fd_4<fd_direction::y, double>(ref_lapse, 0.0, x, y, z);
  const auto d_ref_lapse_dz = fd_4<fd_direction::z, double>(ref_lapse, 0.0, x, y, z);

  EXPECT_NEAR(metric->grad_lapse(0.0, x, y, z)[0], d_ref_lapse_dx, fd_comp_tol);
  EXPECT_NEAR(metric->grad_lapse(0.0, x, y, z)[1], d_ref_lapse_dy, fd_comp_tol);
  EXPECT_NEAR(metric->grad_lapse(0.0, x, y, z)[2], d_ref_lapse_dz, fd_comp_tol);
}

TEST_F(Kerr_Schild_Kerr_Tests, grad_upper_shift_correcness) {
  using namespace grlensing_tests;
  using namespace grlensing;

  const double M = 1.0;
  const double a = M / 2;

  const double x = (*shared_coord_distrib)(*shared_random_engine);
  const double y = (*shared_coord_distrib)(*shared_random_engine);
  const double z = (*shared_coord_distrib)(*shared_random_engine);

  const auto &metric = shared_kernel->get_metric_server().get_metric("Kerr-Schild Kerr");

  // Shift from reference metric
  const auto ref_u_shift_0 = [=](double, double x, double y, double z) -> double {
    const auto ref_uu_g = uu_g(M, a, x, y, z);
    return -ref_uu_g[0][1] / ref_uu_g[0][0];
  };
  const auto ref_u_shift_1 = [=](double, double x, double y, double z) -> double {
    const auto ref_uu_g = uu_g(M, a, x, y, z);
    return -ref_uu_g[0][2] / ref_uu_g[0][0];
  };
  const auto ref_u_shift_2 = [=](double, double x, double y, double z) -> double {
    const auto ref_uu_g = uu_g(M, a, x, y, z);
    return -ref_uu_g[0][3] / ref_uu_g[0][0];
  };

  const auto d_ref_u_shift_0_dx = fd_4<fd_direction::x, double>(ref_u_shift_0, 0.0, x, y, z);
  const auto d_ref_u_shift_1_dx = fd_4<fd_direction::x, double>(ref_u_shift_1, 0.0, x, y, z);
  const auto d_ref_u_shift_2_dx = fd_4<fd_direction::x, double>(ref_u_shift_2, 0.0, x, y, z);

  const auto d_ref_u_shift_0_dy = fd_4<fd_direction::y, double>(ref_u_shift_0, 0.0, x, y, z);
  const auto d_ref_u_shift_1_dy = fd_4<fd_direction::y, double>(ref_u_shift_1, 0.0, x, y, z);
  const auto d_ref_u_shift_2_dy = fd_4<fd_direction::y, double>(ref_u_shift_2, 0.0, x, y, z);

  const auto d_ref_u_shift_0_dz = fd_4<fd_direction::z, double>(ref_u_shift_0, 0.0, x, y, z);
  const auto d_ref_u_shift_1_dz = fd_4<fd_direction::z, double>(ref_u_shift_1, 0.0, x, y, z);
  const auto d_ref_u_shift_2_dz = fd_4<fd_direction::z, double>(ref_u_shift_2, 0.0, x, y, z);

  // log<LogEvent::message>("point ({},{},{})", x, y, z);

  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[0][0], d_ref_u_shift_0_dx, fd_comp_tol);
  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[0][1], d_ref_u_shift_1_dx, fd_comp_tol);
  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[0][2], d_ref_u_shift_2_dx, fd_comp_tol);

  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[1][0], d_ref_u_shift_0_dy, fd_comp_tol);
  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[1][1], d_ref_u_shift_1_dy, fd_comp_tol);
  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[1][2], d_ref_u_shift_2_dy, fd_comp_tol);

  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[2][0], d_ref_u_shift_0_dz, fd_comp_tol);
  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[2][1], d_ref_u_shift_1_dz, fd_comp_tol);
  EXPECT_NEAR(metric->grad_ushift(0.0, x, y, z)[2][2], d_ref_u_shift_2_dz, fd_comp_tol);
}

/**
 * TODO:
 * 1. Test Christoffel symbols
 * 2. Test ll_extrinsic
 * 3. Test ul_extrinsic
 */