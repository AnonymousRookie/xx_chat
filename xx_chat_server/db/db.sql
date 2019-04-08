DROP DATABASE IF EXISTS db_xx_chat;
CREATE DATABASE db_xx_chat DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE db_xx_chat;


# -----------------------------------------------------

DROP TABLE IF EXISTS `t_user`;
CREATE TABLE `t_user` (
    `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户id',
    `name` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '用户名',
    `password` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '密码',
    `created` int(11) unsigned NOT NULL COMMENT '创建时间',
    `updated` int(11) unsigned NOT NULL COMMENT '更新时间',
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

INSERT INTO t_user values(1001, "jason", "123", 0, 0);