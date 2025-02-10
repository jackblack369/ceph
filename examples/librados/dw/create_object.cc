#include <rados/librados.hpp>
#include <iostream>
#include <string>

int main() {
    librados::Rados cluster;
    char cluster_name[] = "ceph";
    char user_name[] = "client.admin";
    char pool_name[] = "dongwei";
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

    // Open an IoCtx for the new pool 
    librados::IoCtx io_ctx;
    ret = cluster.ioctx_create(pool_name, io_ctx);
    if (ret < 0) {
        std::cerr << "Couldn't set up IoCtx! Error " << ret << std::endl;
        return 1;
    }

    std::cout << "IoCtx successfully created for pool: " << pool_name << std::endl;

    // Define object name
    std::string object_name = "my_object";

    // Write data to the object
    std::string data = "Hello, Ceph!";
    librados::bufferlist bl;
    bl.append(data);
    ret = io_ctx.write_full(object_name, bl);

    //ret = io_ctx.write_full(object_name, librados::bufferlist().append(data));
    if (ret < 0) {
        std::cerr << "Failed to write object: " << ret << std::endl;
        return 1;
    }
    std::cout << "Successfully wrote object: " << object_name << std::endl;

    // Set an extended attribute (xattr)
    std::string attr_name = "user.comment";
    std::string attr_value = "This is a test xattr";
    librados::bufferlist commit_bl;
    commit_bl.append(attr_value);

    ret = io_ctx.setxattr(object_name, attr_name.c_str(), commit_bl);
    if (ret < 0) {
        std::cerr << "Failed to set xattr: " << ret << std::endl;
        return 1;
    }
    std::cout << "Successfully set xattr '" << attr_name << "' on object: " << object_name << std::endl;

    return 0;
}