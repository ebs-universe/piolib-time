import setuptools

setuptools.setup(
    name="ebs-time",
    url="https://github.com/chintal/ebs-lib-time",

    author="Chintalagiri Shashank",
    author_email="shashank.chintalagiri@gmail.com",

    description="Python host interface to Time elements of EBS devices",
    long_description=open('README.rst').read(),

    packages=setuptools.find_packages(),

    install_requires=['ebs-modbus', 'ebs-ucdm'],

    setup_requires=['setuptools_scm'],
    use_scm_version={'root': '../', 'relative_to': __file__},

    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Operating System :: POSIX :: Linux',
    ],
)
