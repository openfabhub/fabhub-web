import re

from typing import Optional

from conans import ConanFile, CMake
from conans.tools import load


def read_project_property(property: str) -> Optional[str]:
    try:
        cmake_lists = load("CMakeLists.txt")
        version = re.search(r"project\(.*{} \"(.*?)\"".format(property), cmake_lists, re.S).group(1)
        return version.strip()
    except:
        return None


class MofprintConan(ConanFile):
    name = "mofprint"
    description = read_project_property("DESCRIPTION")
    version = read_project_property("VERSION")
    license = "BSD-3-Clause"
    homepage = "https://github.com/fmorgner/mofprint"
    url = homepage
    scm = {
        "type": "git",
        "url": "https://github.com/fmorgner/mofprint.git",
        "revision": "auto"
    }

    settings = ("compiler",)
    generators = ("cmake",)

    build_requires = (
        "boost/[~=1.72]",
        "lyra/[~=1.2]",
        "spdlog/[~=1.5]",
        "CUTE/[~=2.2]@fmorgner/stable",
        "newtype/[~=1.0]",
    )
    default_options = {
        "boost:bzip2": False,
        "boost:header_only": True,
        "boost:without_atomic": True,
        "boost:without_chrono": True,
        "boost:without_container": True,
        "boost:without_context": True,
        "boost:without_contract": True,
        "boost:without_coroutine": True,
        "boost:without_date_time": True,
        "boost:without_exception": True,
        "boost:without_fiber": True,
        "boost:without_filesystem": True,
        "boost:without_graph_parallel": True,
        "boost:without_graph": True,
        "boost:without_iostreams": True,
        "boost:without_locale": True,
        "boost:without_log": True,
        "boost:without_math": True,
        "boost:without_mpi": True,
        "boost:without_program_options": True,
        "boost:without_python": True,
        "boost:without_random": True,
        "boost:without_regex": True,
        "boost:without_serialization": True,
        "boost:without_stacktrace": True,
        "boost:without_test": True,
        "boost:without_thread": True,
        "boost:without_timer": True,
        "boost:without_type_erasure": True,
        "boost:without_wave": True,
        "boost:zlib": False,
        "spdlog:no_exceptions": True,
        "spdlog:shared": False,
    }

    @property
    def _cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        self._cmake.build()

    def package(self):
        self._cmake.install()