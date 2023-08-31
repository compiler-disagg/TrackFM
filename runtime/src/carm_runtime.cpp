#include "carm_runtime.hpp"
#include "carm.hpp"
#include "carm_loops.hpp"
#include <stdlib.h>

using namespace far_memory;

extern void do_work(void * arg);
CARMRT * carm_rt = nullptr;
bool   carm_instrument = false;
extern void flush_cache();

struct arg_struct {
	long arg_count;
  char ** args;
};

struct arg_struct * cmdlist;

char * conf_path;
char * ip_addr;
char * pname;
char ** pargs;

void init_objects(uint64_t size, uint64_t local_cache_size) {

  printf("%s OBJ SIZE %d\n",__func__, OBJ_SIZE);
  carm_rt->aspace_start = START_ADDR;
  carm_rt->aspace_end  =  carm_rt->aspace_start + size;
  carm_rt->carm_ds_array = new CARM_AIFM_DS(carm_rt->manager, OBJ_SIZE);
  carm_rt->carm_ds_array->obj_size = OBJ_SIZE;                         
  carm_rt->carm_ds_array->ds_id    = 0;
  carm_rt->carm_ds_array->num_objects = (size/carm_rt->carm_ds_array->obj_size);
  carm_rt->carm_ds_array->total_obj_cnt = carm_rt->carm_ds_array->num_objects;

  //preempt_disable();
  carm_rt->carm_ds_array->ptrs_ = (GenericUniquePtr **)malloc(sizeof(GenericUniquePtr *) * TOTAL_OBJECTS);
  //preempt_enable();


  //  carm_rt->carm_ds_array->ptrs_.reset(new GenericUniquePtr *[carm_rt->carm_ds_array->num_objects * 20]);
  //carm_rt->carm_ds_array->free_list_map.reset(new uint64_t (TOTAL_OBJECTS));
  //carm_rt->carm_ds_array->free_list_map.reset(new uint64_t (TOTAL_OBJECTS));
  carm_rt->carm_ds_array->free_list_map = (uint64_t *)malloc(sizeof(uint64_t) * TOTAL_OBJECTS);

  for (uint64_t obj_index = 0; obj_index < carm_rt->carm_ds_array->num_objects; obj_index++) {
    carm_rt->carm_ds_array->ptrs_[obj_index] = nullptr;
  }
  #if 0
  //keep this in another loop as metadata changes
  for (uint64_t obj_index = 0; obj_index < carm_rt->carm_ds_array->num_objects; obj_index++) {
    set_carm_obj_state(carm_rt->carm_ds_array->ptrs_[obj_index]->meta(), obj_index); 
  }
#endif
}

void init_carm_runtime() {
  std::string ip_addr_port(ip_addr);
	printf("rp addr %s\n", ip_addr);
  std::cout << "raddr " << ip_addr_port << std::endl;
  auto raddr = helpers::str_to_netaddr(ip_addr_port);
  carm_rt = new CARMRT();
  carm_rt->manager = FarMemManagerFactory::build(
      local_mem_cache_size, kNumGCThreads,
      new TCPDevice(raddr, kNumConnections, kFarMemSize));
  init_objects(kWorkingSetSize, local_mem_cache_size);
  std::cout<<"size of "<<sizeof(FarMemPtrMeta)<<"\n";

  carm_instrument = true;
}


int cruntime_init(thread_fn_t do_work)
{
  int ret;

  ret = runtime_init(conf_path, do_work, (void *)cmdlist);
  return ret;
}

int get_arg_count() {
	return cmdlist->arg_count; 
}

char ** get_args() {
	return cmdlist->args; 
}

int main(int argc, char **argv) {
	int skip_aifm_args = 3; 
	int pg = 1;

	cmdlist = (struct arg_struct *)malloc(sizeof(struct arg_struct));
  init_carm_meta_obj(); 
  flush_carn_meta_obj();
  init_loops();
  int ret;

  if (argc < 3) {
    std::cerr << "usage: [cfg_file] [ip_addr:port]" << std::endl;
    return -EINVAL;
  }

  pname = (char *)malloc((strlen(argv[0]) + 1) * sizeof(char)) ;
  strcpy(pname, argv[0]);

  conf_path = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char)) ;
  strcpy(conf_path, argv[1]);

  ip_addr = (char *)malloc((strlen(argv[2]) + 1) * sizeof(char)) ;
  strcpy(ip_addr, argv[2]);

  pargs = (char **)malloc((argc + 1) * sizeof(char *));
	memset((void *)pargs, 0, (argc + 1) * sizeof(char *));

	pargs[0] = argv[0]; 

	for (int i = skip_aifm_args; i < argc; i++) {
		pargs[pg] = argv[i];
		pg++;
	}

	cmdlist->args = pargs;
	cmdlist->arg_count = argc - 2;
	
  ret = cruntime_init(do_work);
  if (ret) {
    std::cerr << "failed to start runtime" << std::endl;
    return ret;
  }

  return 0;
}
