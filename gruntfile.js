/// <binding ProjectOpened='watch:tasks' />
module.exports = function (grunt) {
    grunt.initConfig({
        shell: {
            cleanall: {
                command: "bash -c 'make clean compile install'"
            },
            all: {
                command: "bash -c 'make compile install'"
            },
            build: {
                command: "bash -c 'make compile'"
            },
            clean: {
                command: "bash -c 'make clean'"
            },
            install: {
                command: "bash -c 'make install'"
            }
        },
        copy: {
            dev: {
                files: [
                    {
                        src: "kernel.sys",
                        dest: "D:\\kernel.sys"
                    }
                ]
            }
        },
        watch: {
            files: ["kernel.sys"],
            tasks: ["copy:dev"]
        }
    });

    grunt.loadNpmTasks("grunt-shell");
    grunt.loadNpmTasks("grunt-contrib-watch");
    grunt.loadNpmTasks("grunt-contrib-copy");
}