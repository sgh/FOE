
create table users (
	id integer PRIMARY KEY AUTO_INCREMENT,
	name varchar(30) unique,
	password varchar(64),
	clanid integer
);

create table products (
	id_user    integer,
	product    integer,
	factories  int,
	bonus      int,
	primary key (id_user,product)
);

create table clans (
	id integer unique auto_increment primary key,
	name varchar(64)
);
