
create table users (
	id integer PRIMARY KEY AUTO_INCREMENT,
	name varchar(30) unique,
	password varchar(64)
);

create table products (
	id_user    integer,
	product    integer,
	factories  int,
	bonus      int,
	primary key (id_user,product)
);



