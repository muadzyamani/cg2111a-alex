from setuptools import setup, find_packages
setup(
    name='pubsub',
    version='0.1.0',
    packages=find_packages(include=[ 'pubsub.pubsub.*']),
    author='EPP2 Teaching Team',
    python_requires='>=3.6',
    install_requires=[
        'numpy',
        'matplotlib',
        'pyserial'
    ],
    description='EPP2 pubsub Library',
)
