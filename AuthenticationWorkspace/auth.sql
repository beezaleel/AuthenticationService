CREATE TABLE `user` (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `last_login` timestamp NOT NULL,
  `created_date` datetime DEFAULT NULL,
  `userid` bigint NOT NULL,
  PRIMARY KEY (`id`),
  KEY `IdIndex` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci

CREATE TABLE `web_auth` (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `email` varchar(255) DEFAULT NULL,
  `salt` char(64) DEFAULT NULL,
  `hashed_password` char(64) DEFAULT NULL,
  `userid` bigint DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `userIndex` (`userid`),
  KEY `emailIndex` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci