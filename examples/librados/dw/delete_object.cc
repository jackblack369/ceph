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

    // Define object name and xattr name
    std::string object_name = "my_object";
    std::string attr_name = "user.comment";

    /*
     * Read the object back asynchronously.
     */
    {
        librados::bufferlist read_buf;
        int read_len = 4194304;

        //Create I/O Completion.
        librados::AioCompletion *read_completion = librados::Rados::aio_create_completion();

        //Send read request.
        ret = io_ctx.aio_read(object_name, read_completion, &read_buf, read_len, 0);
        if (ret < 0) {
                std::cerr << "Couldn't start read object! error " << ret << std::endl;
                exit(EXIT_FAILURE);
        }

        // Wait for the request to complete, and check that it succeeded.
        read_completion->wait_for_complete();
        ret = read_completion->get_return_value();
        if (ret < 0) {
                std::cerr << "Couldn't read object! error " << ret << std::endl;
                exit(EXIT_FAILURE);
        } else {
                std::cout << "Read object asynchronously with contents.\n"
                << read_buf.c_str() << std::endl;
        }
    }

    // Read the xattr
    librados::bufferlist read_buf;
    ret = io_ctx.getxattr(object_name, attr_name.c_str(), read_buf);
    if (ret < 0) {
        std::cerr << "Failed to read xattr: " << ret << std::endl;
    } else {
        std::string attr_value(read_buf.c_str(), read_buf.length());
        std::cout << "Read xattr '" << attr_name << "': " << attr_value << std::endl;
    }

    // Remove the xattr
    ret = io_ctx.rmxattr(object_name, attr_name.c_str());
    if (ret < 0) {
        std::cerr << "Failed to remove xattr: " << ret << std::endl;
    } else {
        std::cout << "Successfully removed xattr '" << attr_name << "' from object: " << object_name << std::endl;
    }

    // Delete the object
    ret = io_ctx.remove(object_name);
    if (ret < 0) {
        std::cerr << "Failed to remove object: " << ret << std::endl;
    } else {
        std::cout << "Successfully removed object: " << object_name << std::endl;
    }
   
    return 0;
}