#
# Cookbook Name:: crosscompiler
# Recipe:: default

packages = value_for_platform(
  "default" => ['curl', 'nasm']
)

packages.each do |pkg|
  package pkg
end

# Build binutils
bash "binutils" do
  cwd "/tmp/"
  code <<-EOH
    export PATH="#{node['cc32']['prefix']}/bin:$PATH"

    rm -rf binutils

    mkdir binutils
    pushd binutils
    curl http://ftp.gnu.org/gnu/binutils/#{node['cc32']['binutils']}.tar.gz -o #{node['cc32']['binutils']}.tar.gz
    
    if [[ ! -f "#{node['cc32']['binutils']}.tar.gz" ]]
    then
        echo "[ERROR]: Could not download binutils"
        exit 1
    fi

    tar -xzf #{node['cc32']['binutils']}.tar.gz
    mkdir build-binutils
    pushd build-binutils

   ../#{node['cc32']['binutils']}/configure --target=#{node['cc32']['target']} --prefix="#{node['cc32']['prefix']}" --disable-nls
   make
   make install

   EOH
end

bash "get_sources" do
   cwd "/tmp/"
   code <<-EOH
    export PATH="#{node['cc32']['prefix']}/bin:$PATH"

    rm -rf gcc

    mkdir gcc
    pushd gcc

   curl http://gcc.parentingamerica.com/releases/#{node['cc32']['gcc']}/#{node['cc32']['gcc']}.tar.gz -o #{node['cc32']['gcc']}.tar.gz

   if [[ ! -f #{node['cc32']['gcc']}.tar.gz ]]
   then
      echo "[ERROR]: Could not download GCC"
      exit 1
   fi

   curl https://ftp.gnu.org/gnu/gmp/#{node['cc32']['gmp']}.tar.bz2 -o #{node['cc32']['gmp']}.tar.bz2
   curl http://www.mpfr.org/mpfr-current/#{node['cc32']['mpfr']}.tar.gz -o #{node['cc32']['mpfr']}.tar.gz
   curl http://multiprecision.org/mpc/download/#{node['cc32']['mpc']}.tar.gz -o #{node['cc32']['mpc']}.tar.gz
   curl http://ftp.gnu.org/pub/gnu/libiconv/#{node['cc32']['libiconv']}.tar.gz -o #{node['cc32']['libiconv']}.tar.gz
   
   EOH
end

bash "build_gcc" do
   cwd "/tmp/gcc"
   code <<-EOH
      export PATH="#{node['cc32']['prefix']}/bin:$PATH"

   tar -xzf #{node['cc32']['gcc']}.tar.gz
   tar -xjf #{node['cc32']['gmp']}.tar.bz2
   tar -xzf #{node['cc32']['mpfr']}.tar.gz
   tar -xzf #{node['cc32']['mpc']}.tar.gz
   tar -xzf #{node['cc32']['libiconv']}.tar.gz

   mv #{node['cc32']['gmp']} #{node['cc32']['gcc']}/gmp
   mv #{node['cc32']['mpfr']} #{node['cc32']['gcc']}/mpfr
   mv #{node['cc32']['mpc']} #{node['cc32']['gcc']}/mpc
   mv #{node['cc32']['libiconv']} #{node['cc32']['gcc']}/libiconv

   mkdir build-gcc
   pushd build-gcc

   ../#{node['cc32']['gcc']}/configure --target=#{node['cc32']['target']} --prefix="#{node['cc32']['prefix']}" --disable-nls --enable-languages=c,c++ --without-headers
   make all-gcc
   make all-target-gcc
   make all-target-libgcc
   make install-gcc
   make install-target-libgcc

   EOH
end
