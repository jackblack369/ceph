#include <rados/librados.hpp>
#include <iostream>

int main() {
    librados::Rados cluster;
    char cluster_name[] = "ceph";
    char user_name[] = "client.admin";
    uint64_t flags = 0;
    int ret;

    // Initialize and connect to Ceph cluster
    ret = cluster.init2(user_name, cluster_name, flags);
    if (ret < 0) {
        std::cerr << "Failed to initialize cluster: " << ret << std::endl;
        return 1;
    }
    ret = cluster.conf_read_file("/etc/ceph/ceph.conf");
    ret = cluster.connect();
    if (ret < 0) {
        std::cerr << "Failed to connect to cluster: " << ret << std::endl;
        return 1;
    }

    // Define pool name
    const char *pool_name = "dongwei";

    // Create the pool
    ret = cluster.pool_create(pool_name);
    if (ret < 0 && ret != -EEXIST) {  // Ignore error if pool already exists
        std::cerr << "Failed to create pool: " << ret << std::endl;
        return 1;
    } else if (ret == -EEXIST) {
        std::cout << "Pool '" << pool_name << "' already exists." << std::endl;
    } else {
        std::cout << "Successfully created pool: " << pool_name << std::endl;
    }

    // Open an IoCtx for the new pool
    /** 
    librados::IoCtx io_ctx;
    ret = cluster.ioctx_create(pool_name, io_ctx);
    if (ret < 0) {
        std::cerr << "Couldn't set up IoCtx! Error " << ret << std::endl;
        return 1;
    }

    std::cout << "IoCtx successfully created for pool: " << pool_name << std::endl;
    */

    return 0;
}