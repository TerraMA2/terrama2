define([], () => {
  class StorageService {
    constructor(BaseService) {
      this.BaseService = BaseService;
    }

    async init(restriction = {}) {
      const { BaseService } = this;

      try {
        const response = await BaseService.$request(`${BASE_URL}api/storages`, 'GET', { params: restriction });

        this.model = response.data;
      } catch (err) {
        throw err;
      }

    }

    list(restriction) {
      return this.BaseService.$filter('filter')(this.model, restriction);
    }

    async get(storageId) {
      const { BaseService } = this;

      const response = await BaseService.$request(`${BASE_URL}api/storages/${storageId}`, 'GET');

      return response.data;
    }

    async save(storageObj) {
      const { BaseService } = this;

      try {
        const response = await BaseService.$request(`${BASE_URL}api/storages`, 'POST', { data: storageObj });

        return response.data;
      } catch (err) {
        throw new Error(err.message);
      }
    }
  }

  StorageService.$inject = ['BaseService'];

  return StorageService;
});