const expect = require('chai').expect;
const sinon = require('sinon');
const fs = require('fs');
const child_process = require('child_process');

// TerraMA2
const DataManager = require('./../../core/DataManager');
const FileExtract = require('./../../core/FileExtract');
const Exportation = require('./../../core/Exportation');
const { MultipleShapeFileFoundError, NoShapeFileFoundError, ShapeImporter } = require('./../../core/ShapeImporter');

describe('ShapeImporter test', () => {
  const temporaryDir = '/tmp/';
  const srid = 4326;
  const importer = new ShapeImporter(temporaryDir, srid, 'LATIN1');

  describe('#toDatabase', () => {
    let unzipStub = null;
    let nodeChildProcessStud = null;
    let statementStub = null;

    beforeEach(() => {
      unzipStub = sinon.stub(FileExtract, 'unzip').returns({
        getEntries: () => [{ entryName: `${temporaryDir}/fake.shp` }]
      });

      importer.unzip('/fake/path');
      expect(importer.getFiles()).to.have.length(1);

      nodeChildProcessStud = sinon.stub(child_process, 'exec').yields(null, 'fake');

      statementStub = sinon.stub(Exportation.prototype, 'getPsqlString').returns('');
    });

    afterEach(() => {
      unzipStub.restore();
      nodeChildProcessStud.restore();
      statementStub.restore();
    });

    it('exports shapefile to database', async () => {
      const tableStub = sinon.stub(Exportation.prototype, 'tableExists').onCall(0).resolves(false).onCall(1).resolves(true);

      const undefinedResult = await importer.toDatabase('fakeTable', 1);

      tableStub.restore();

      expect(undefinedResult).to.be.undefined;
    });

    it('throws error when table already exists', async () => {
      const tableStub = sinon.stub(Exportation.prototype, 'tableExists').resolves(true);

      try {
        await importer.toDatabase('fakeTable', 1);
      } catch (err) {
        expect(err.message).to.be.equal('Table already exists!');
      } finally {
        tableStub.restore();
      }
    });

    it('throws error when data provider does not exist', async () => {
      statementStub.restore();
      const tableStub = sinon.stub(Exportation.prototype, 'getPsqlString').rejects('Data Provider Not Found');

      try {
        await importer.toDatabase('fakeTable', 1);
      } catch (err) {
        expect(err.toString()).to.be.equal('Data Provider Not Found');
      } finally {
        tableStub.restore();
      }
    });
  });

  describe('#unzip', () => {
    it('extracts shape from zip', () => {
      const stub = sinon.stub(FileExtract, 'unzip').returns({
        getEntries: () => [{ entryName: `${temporaryDir}/fake.shp` }]
      });

      importer.unzip('/fakePath/fake.zip');

      expect(importer.getFiles()).to.have.lengthOf(1);
      expect(importer.getFiles()).to.include.members([`${temporaryDir}/fake.shp`]);

      stub.restore();
    });

    it('throws error when extract zip which not contains shape', () => {
      const stub = sinon.stub(FileExtract, 'unzip').returns({
        getEntries: () => [{ entryName: `${temporaryDir}/shape.txt` }]
      });

      const importer = new ShapeImporter(temporaryDir, srid, 'LATIN1');

      try {
        importer.unzip('/fakePath/fake.zip');
      } catch (err) {
        expect(err).to.be.instanceOf(NoShapeFileFoundError);
      } finally {
        stub.restore();
      }
    });

    it('throws error when extract zip which contains more than one shapefile', () => {
      const stub = sinon.stub(FileExtract, 'unzip').returns({
        getEntries: () => [
          { entryName: `${temporaryDir}/shape-1.shp` },
          { entryName: `${temporaryDir}/shape-2.shp` }
        ]
      });

      const importer = new ShapeImporter(temporaryDir, srid, 'LATIN1');

      try {
        importer.unzip('/fakePath/fake.zip');
      } catch (err) {
        expect(err).to.be.instanceOf(MultipleShapeFileFoundError);
      } finally {
        stub.restore();
      }
    });

  });

  describe('#toDataProvider', () => {
    let unzipStub = null;
    let nodeChildProcessStud = null;

    const successDataProviderStub = () => (
      sinon.stub(DataManager, 'getDataProvider').resolves({ id: 1, name: 'Fake DataProvider', uri: 'file:///fakePath' })
    )

    beforeEach(() => {
      unzipStub = sinon.stub(FileExtract, 'unzip').returns({
        getEntries: () => [{ entryName: `${temporaryDir}/fake.shp` }]
      });

      importer.unzip('/fake/path');
      expect(importer.getFiles()).to.have.length(1);

      nodeChildProcessStud = sinon.stub(child_process, 'exec').yields(null, 'fake');
    });

    afterEach(() => {
      unzipStub.restore();
      nodeChildProcessStud.restore();
    });

    it('exports shapefile to FILE data provider', async () => {
      const dataManagerStub = successDataProviderStub();
      const fsStub = sinon.stub(fs, 'existsSync').onCall(0).returns(true).onCall(1).returns(false);
      const exportationStub = sinon.stub(Exportation.prototype, 'copyShpFiles').returns(undefined);

      const undefinedResult = await importer.toDataProvider(1, 'fakeShape.shp');

      exportationStub.restore();
      fsStub.restore();
      dataManagerStub.restore();

      expect(undefinedResult).to.be.undefined;
    });

    it('throws error when data provider not found', async () => {
      const dataManagerStub = sinon.stub(DataManager, 'getDataProvider').rejects(new Error('Not found'));

      try {
        await importer.toDataProvider(1, 'fakeShape.shp');
      } catch (err) {
        expect(err.message).to.be.equal('Not found');
      } finally {
        dataManagerStub.restore();
      }
    });

    it('throws error when there is invalid path in data provider', async () => {
      const dataManagerStub = successDataProviderStub();
      const fsStub = sinon.stub(fs, 'existsSync').returns(false);

      try {
        await importer.toDataProvider(1, 'fakeShape.shp');
      } catch (err) {
        expect(err.message).to.be.equal('Invalid data provider path!');
      }

      fsStub.restore();
      dataManagerStub.restore();
    });

    it('throws error when shapefile already exists', async () => {
      const dataManagerStub = successDataProviderStub();
      const fsStub = sinon.stub(fs, 'existsSync').onCall(0).returns(true).onCall(1).returns(true);

      try {
        await importer.toDataProvider(1, 'fakeShape.shp');
      } catch (err) {
        expect(err.message).to.be.equal('File already exists!');
      }

      fsStub.restore();
      dataManagerStub.restore();
    });
  });


});