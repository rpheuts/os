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
    export PATH="#{node['cc64']['prefix']}/bin:$PATH"

    mkdir binutils
    pushd binutils
    curl http://ftp.gnu.org/gnu/binutils/#{node['cc64']['binutils']}.tar.gz -o #{node['cc64']['binutils']}.tar.gz
    
    if [[ ! -f "#{node['cc64']['binutils']}.tar.gz" ]]
    then
        echo "[ERROR]: Could not download binutils"
        exit 1
    fi

    tar -xzf #{node['cc64']['binutils']}.tar.gz
    mkdir build-binutils
    pushd build-binutils

   ../#{node['cc64']['binutils']}/configure --target=#{node['cc64']['target']} --prefix="#{node['cc64']['prefix']}" --disable-nls
   make
   make install

   EOH
end

bash "get_sources" do
   cwd "/tmp/"
   code <<-EOH
    export PATH="#{node['cc64']['prefix']}/bin:$PATH"

    mkdir gcc
    pushd gcc

   curl http://gcc.parentingamerica.com/releases/#{node['cc64']['gcc']}/#{node['cc64']['gcc']}.tar.gz -o #{node['cc64']['gcc']}.tar.gz

   if [[ ! -f #{node['cc64']['gcc']}.tar.gz ]]
   then
      echo "[ERROR]: Could not download GCC"
      exit 1
   fi

   curl https://ftp.gnu.org/gnu/gmp/#{node['cc64']['gmp']}.tar.bz2 -o #{node['cc64']['gmp']}.tar.bz2
   curl http://www.mpfr.org/mpfr-current/#{node['cc64']['mpfr']}.tar.gz -o #{node['cc64']['mpfr']}.tar.gz
   curl http://multiprecision.org/mpc/download/#{node['cc64']['mpc']}.tar.gz -o #{node['cc64']['mpc']}.tar.gz
   curl http://ftp.gnu.org/pub/gnu/libiconv/#{node['cc64']['libiconv']}.tar.gz -o #{node['cc64']['libiconv']}.tar.gz
   
   EOH
end

bash "build_gcc" do
   cwd "/tmp/gcc"
   code <<-EOH
      export PATH="#{node['cc64']['prefix']}/bin:$PATH"

   tar -xzf #{node['cc64']['gcc']}.tar.gz
   tar -xjf #{node['cc64']['gmp']}.tar.bz2
   tar -xzf #{node['cc64']['mpfr']}.tar.gz
   tar -xzf #{node['cc64']['mpc']}.tar.gz
   tar -xzf #{node['cc64']['libiconv']}.tar.gz

   mv #{node['cc64']['gmp']} #{node['cc64']['gcc']}/gmp
   mv #{node['cc64']['mpfr']} #{node['cc64']['gcc']}/mpfr
   mv #{node['cc64']['mpc']} #{node['cc64']['gcc']}/mpc
   mv #{node['cc64']['libiconv']} #{node['cc64']['gcc']}/libiconv

   mkdir build-gcc
   pushd build-gcc

   ../#{node['cc64']['gcc']}/configure --target=#{node['cc64']['target']} --prefix="#{node['cc64']['prefix']}" --disable-nls --enable-languages=c,c++ --without-headers
   make all-gcc
   make all-target-gcc
   make all-target-libgcc
   make install-gcc
   make install-target-libgcc

   EOH
end
