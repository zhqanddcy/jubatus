#include <pficommon/lang/shared_ptr.h>
#include <msgpack.hpp>

#include "server.hpp"
#include "regression/regression.hpp"
#include "fv_converter/datum.hpp"
#include "fv_converter/datum_to_fv_converter.hpp"
#include "fv_converter/converter_config.hpp"
#include "regression/regression_factory.hpp"
#include "storage/local_storage_mixture.hpp"
#include "diffv.hpp"

SET_PROGNAME("regression");

namespace jubatus {
namespace regression {

struct config_data {
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);
};

struct model {
  datum_to_fv_converter conv;
  pfi::lang::shared_ptr<regression_base> regression;
  pfi::lang::shared_ptr<storage::storage_base> storage;
  config_data conf;
};

void train(model* m, const std::pair<float, datum>& labeled) {
  sfv_t fv;
  m->conv.convert(labeled.second, fv);
  m->regression->train(fv, labeled.first);
}

float estimate(const model* m, const datum& dat) {
  sfv_t fv;
  const_cast<datum_to_fv_converter&>(m->conv).convert(dat, fv);
  return m->regression->estimate(fv);
}

void set_config(model* m, const config_data& conf) {
  m->conf = conf;
  initialize_converter(conf.converter, m->conv);
  m->storage.reset(new storage::local_storage_mixture());
  regression_base* r = regression_factory().create_regression(conf.method,
                                                              m->storage.get());
  m->regression.reset(r);
}

config_data get_config(const model* m, int){
  return m->conf;
}

int save(const model*, const std::string& id){
  //FIXME
  return -1;
}
void load(model*, const std::string& id){
  //FIXME
}

diffv get_diff(const model*){  //FIXME
  diffv d;
  return d;
}
int mix(const model*, const diffv& lhs, const diffv& rhs, diffv& acc){
  return -1;//FIXME
}
int put_diff(model*, const diffv&){  //FIXME
  return -1;
}


} //regression
} //jubatus

using namespace std;

int main(int argc, char* argv[]) {
  const jubatus::server_argv a(argc, argv);
  jubatus::server<jubatus::regression::model, jubatus::diffv> serv(a);

  serv.register_update<pair<float, jubatus::datum> >(
      "train", &jubatus::regression::train);
  serv.register_analysis<jubatus::datum, float>(
      "estimate", &jubatus::regression::estimate);
  
  serv.register_update<jubatus::regression::config_data>(
      "set_config", &jubatus::regression::set_config);
  serv.register_analysis<int, jubatus::regression::config_data>(
      "get_config", &jubatus::regression::get_config);

  serv.register_analysis<std::string, int>(
      "save", &jubatus::regression::save);
  serv.register_update<std::string>(
      "load", &jubatus::regression::load);

  // FIXME: we need bind 'get_status' API here.

#ifdef HAVE_ZOOKEEPER_H
  pfi::lang::function<jubatus::diffv(const jubatus::regression::model*)> g(&jubatus::regression::get_diff);
  pfi::lang::function<int(const jubatus::regression::model*,
                          const jubatus::diffv&,
                          const jubatus::diffv&,
                          jubatus::diffv&)>  m(&jubatus::regression::mix);
  pfi::lang::function<int(jubatus::regression::model*, const jubatus::diffv&)> p(&jubatus::regression::put_diff);
  serv.start_mixer(g, m, p);

#endif

  return serv.serv(9199, a.timeout);
}
